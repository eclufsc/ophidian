#include "circuit_canvas.h"

#include <QDebug>
#include <QMouseEvent>
#include <QKeyEvent>

namespace ophidian {
namespace gui {

circuit_canvas::circuit_canvas(QWidget *parent) :
    QSFMLCanvas(parent),
    m_index(m_canvas),
    m_state(nullptr),
    m_camera(sf::FloatRect(0,0,51599.25,34200.0))
{
    clear();
}

circuit_canvas::~circuit_canvas()
{
}

void circuit_canvas::state(canvas_states::state *state)
{
    m_state = state;
}

quad circuit_canvas::drawQuad(const geometry::point<double> &p1, const geometry::point<double> &p2, const geometry::point<double> &p3, const geometry::point<double> &p4)
{
    auto quad = m_canvas.quad_create(p1, p2, p3, p4);
    m_index.quad_create(quad);
    return quad;
}

quad circuit_canvas::quadAt(const sf::Vector2f &point) const
{
    return m_index.quads_containing({point.x, point.y}).front();
}

sf::Vector2f circuit_canvas::mapPixelToCamera(const sf::Vector2i &pixel) const
{
    return mapPixelToCoords(pixel, m_camera);
}

bool circuit_canvas::hasAnimation() const
{
    return m_canvas.has_animation();
}

void circuit_canvas::animate(batch_animation *animation)
{
    m_canvas.quads_animate(animation);
}

const drawable_batch<4> &circuit_canvas::quadsBatch()
{
    return m_canvas.quads();
}

void circuit_canvas::select(quad the_quad)
{

}

void circuit_canvas::unselect(quad the_quad)
{

}

void circuit_canvas::indexQuad(quad the_quad)
{
    m_index.quad_create(the_quad);
}

void circuit_canvas::unindexQuad(quad the_quad)
{
    m_index.quad_remove(the_quad);
}

bool circuit_canvas::hasQuad(const sf::Vector2f &point) const
{
    return !m_index.quads_containing({point.x, point.y}).empty();
}

void circuit_canvas::setFixed(quad the_quad)
{
    m_non_movable_quads.insert(the_quad);

}

void circuit_canvas::dropQuad(quad the_quad)
{
    m_index.quad_remove(the_quad);
    m_index.quad_create(the_quad);
}

void circuit_canvas::eraseQuad(quad the_quad)
{
    m_index.quad_remove(the_quad);
    erase(the_quad);
}

bool circuit_canvas::isFixed(quad the_quad) const
{
    return m_non_movable_quads.find(the_quad) != m_non_movable_quads.end();
}

wire_quad circuit_canvas::drawRect(quad from_quad)
{
    auto quad_points = m_canvas.quad_points(from_quad);
    wire_quad the_rect;
    for(int i = 0; i < 4; ++i)
    {
        the_rect.lines[i] = m_canvas.line_create({quad_points[i].position.x, quad_points[i].position.y}, {quad_points[(i+1)%4].position.x, quad_points[(i+1)%4].position.y});
        m_canvas.paint(the_rect.lines[i], sf::Color::Black);
    }
    return the_rect;
}

wire_quad circuit_canvas::drawRect(const geometry::point<double> &p1, const geometry::point<double> &p2, const geometry::point<double> &p3, const geometry::point<double> &p4)
{
    wire_quad the_rect;
    the_rect.lines[0] = m_canvas.line_create(p1, p2);
    the_rect.lines[1] = m_canvas.line_create(p2, p3);
    the_rect.lines[2] = m_canvas.line_create(p3, p4);
    the_rect.lines[3] = m_canvas.line_create(p4, p1);
    for(int i = 0; i < 4; ++i)
        m_canvas.paint(the_rect.lines[i], sf::Color::Black);
    return the_rect;
}

line circuit_canvas::drawLine(const geometry::point<double> &p1, const geometry::point<double> &p2)
{
    return m_canvas.line_create(p1, p2);
}

void circuit_canvas::cameraCenter(const sf::Vector2f &position)
{
    m_camera.setCenter(position);
}

void circuit_canvas::moveCamera(const sf::Vector2f &delta)
{
    m_camera.move(delta);
}

sf::Vector2f circuit_canvas::cameraSize() const
{
    return m_camera.getSize();
}

void circuit_canvas::cameraSize(const sf::Vector2f &new_size)
{
    m_camera.setSize(new_size);
}

void circuit_canvas::zoom(double value)
{
    m_camera.zoom(value);
}

void circuit_canvas::reset()
{
    if(m_state)
        delete m_state;
    state(new canvas_states::idle(this));
}

void circuit_canvas::save_to_file(const std::string &filename)
{
    sf::RenderTexture texture;
    texture.create(1920, 1080);
    render(texture);
    texture.getTexture().copyToImage().saveToFile(filename);
}

void circuit_canvas::resizeEvent(QResizeEvent *e)
{
    sf::RenderWindow::create((sf::WindowHandle) winId());
}

void circuit_canvas::OnUpdate()
{
    m_fps.update();
    if(m_canvas.has_animation() && !m_save_state){
        m_save_state = m_state;
        m_state = new canvas_states::blocked(this);
    }
    m_canvas.update();
    if(!m_canvas.has_animation() && m_save_state){
        delete m_state;
        m_state = m_save_state;
        m_save_state = nullptr;
    }
    render(*this);
    QSFMLCanvas::update();
}

void circuit_canvas::wheelEvent(QWheelEvent *e)
{
    m_state->wheelEvent(e);
}

void circuit_canvas::keyPressEvent(QKeyEvent *e)
{
    m_state->keyPressEvent(e);
}

void circuit_canvas::mousePressEvent(QMouseEvent *e)
{
    m_state->mousePressEvent(e);
}

void circuit_canvas::mouseMoveEvent(QMouseEvent *e)
{
    m_state->mouseMoveEvent(e);
}

void circuit_canvas::mouseReleaseEvent(QMouseEvent *e)
{
    m_state->mouseReleaseEvent(e);
}

void circuit_canvas::clear()
{
    m_canvas.clear();
    m_index.clear();
    m_save_state = nullptr;
    m_non_movable_quads.clear();
    reset();
}

void circuit_canvas::clear(const sf::Color &color)
{
    QSFMLCanvas::clear(color);
}

void circuit_canvas::render(sf::RenderTarget &target)
{
    target.setView(m_camera);
    target.clear(sf::Color::White);
    target.draw(m_canvas);
    target.setView(target.getDefaultView());
    target.draw(m_fps);
}

canvas_states::idle::idle(circuit_canvas *canvas) :
    state(canvas)
{
}

void canvas_states::idle::mousePressEvent(QMouseEvent *e)
{
    sf::Vector2i pixelCoord{e->pos().x(), e->pos().y()};
    sf::Vector2f viewCoord{m_canvas->mapPixelToCamera(pixelCoord)};
    bool hasQuad{m_canvas->hasQuad(viewCoord)};
    qDebug() << "canvas_states::idle::mousePressEvent() " << viewCoord.x << ", " << viewCoord.y;
    if(hasQuad)
    {
        if(m_canvas->isFixed(m_canvas->quadAt(viewCoord)))
            m_canvas->state(new selected(m_canvas, m_canvas->quadAt(viewCoord)));
        else
            m_canvas->state(new dragging(m_canvas, viewCoord));
        delete this;
    }

}

canvas_states::dragging::dragging(circuit_canvas *canvas, const sf::Vector2f &pos) :
    state(canvas),
    m_initial(pos),
    m_moved(false)
{
    m_quad = m_canvas->quadAt(pos);
}

void canvas_states::dragging::mouseMoveEvent(QMouseEvent *e)
{
    sf::Vector2i pixelCoord{e->pos().x(), e->pos().y()};
    sf::Vector2f viewCoord{m_canvas->mapPixelToCamera(pixelCoord)};
    sf::Vector2f delta(viewCoord.x-m_initial.x, viewCoord.y-m_initial.y);
    sf::Transform translation;
    translation.translate(delta.x, delta.y);
    m_canvas->transform(m_quad, translation);
    m_initial = viewCoord;
    m_moved = true;
}

void canvas_states::dragging::mouseReleaseEvent(QMouseEvent *e)
{
    if(m_moved)
        m_canvas->dropQuad(m_quad);
    m_canvas->state(new selected(m_canvas, m_quad));
    delete this;
}

canvas_states::selected::selected(circuit_canvas *canvas, quad the_quad) :
    state(canvas),
    m_wirequad(m_canvas->drawRect(the_quad)),
    m_quad(the_quad)
{
    m_canvas->select(m_quad);
}

canvas_states::selected::~selected()
{
    m_canvas->erase(m_wirequad);
}

void canvas_states::selected::mousePressEvent(QMouseEvent *e)
{
    sf::Vector2i pixelCoord{e->pos().x(), e->pos().y()};
    sf::Vector2f viewCoord{m_canvas->mapPixelToCamera(pixelCoord)};
    bool hasQuad{m_canvas->hasQuad(viewCoord)};
    qDebug() << "canvas_states::selected::mousePressEvent() " << viewCoord.x << ", " << viewCoord.y;
    m_canvas->unselect(m_quad);
    if(hasQuad)
    {
        if(m_canvas->isFixed(m_canvas->quadAt(viewCoord)))
            m_canvas->state(new selected(m_canvas, m_canvas->quadAt(viewCoord)));
        else
            m_canvas->state(new dragging(m_canvas, viewCoord));
    }
    else
        m_canvas->state(new idle(m_canvas));
    delete this;
}

void canvas_states::selected::keyPressEvent(QKeyEvent *e)
{
    state::keyPressEvent(e);
    if(e->key() == Qt::Key::Key_Delete)
    {
        m_canvas->eraseQuad(m_quad);
        m_canvas->state(new idle(m_canvas));
        delete this;
    }
}

void canvas_states::state::wheelEvent(QWheelEvent *e)
{
    if(e->delta() > 0)
        m_canvas->zoom(1.f/1.1f);
    else if(e->delta() < 0)
        m_canvas->zoom(1.1f);
}

void canvas_states::state::keyPressEvent(QKeyEvent *e)
{
    switch(e->key())
    {
    case Qt::Key::Key_W:
    case Qt::Key::Key_Up:
        m_canvas->moveCamera(sf::Vector2f(0.0f, -m_canvas->cameraSize().y*.1));
        break;
    case Qt::Key::Key_S:
    case Qt::Key::Key_Down:
        m_canvas->moveCamera(sf::Vector2f(0.0f, m_canvas->cameraSize().y*.1));
        break;
    case Qt::Key::Key_D:
    case Qt::Key::Key_Right:
        m_canvas->moveCamera(sf::Vector2f(m_canvas->cameraSize().x*.1, 0.0f));
        break;
    case Qt::Key::Key_A:
    case Qt::Key::Key_Left:
        m_canvas->moveCamera(sf::Vector2f(-m_canvas->cameraSize().x*.1, 0.0f));
        break;
    }
}

canvas_states::blocked::blocked(circuit_canvas *canvas) :
    state(canvas)
{

}


}
}
