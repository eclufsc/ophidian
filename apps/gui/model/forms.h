#ifndef FORMS_H
#define FORMS_H

#include <ophidian/circuit/Netlist.h>

class Form
{

public:
    //! Form Constructor
    /*!
       \brief Constructs a Form
     */
    Form();

    //! Form Constructor
    /*!
       \brief Constructs a Form
       \param id Form identifier.
     */
    explicit Form(uint32_t id);

    //! Form Destructor
    /*!
       \brief Destroys the Form, including its properties
     */
    ~Form();

    uint32_t mId;
};

class Line : public Form {
public:
    //! Line Constructor
    /*!
       \brief Constructs a Line
     */
    Line();

    //! Line Destructor
    /*!
       \brief Destroys the Line, including its properties
     */
    ~Line();
};

class Quad : public Form {
public:
  using cell_type = ophidian::circuit::Netlist::cell_instance_type;
    //! Quad Constructor
    /*!
       \brief Constructs a Quad
     */
    Quad();

    //! Quad Constructor
    /*!
       \brief Constructs a Quad
       \param cell A circuit cell
     */
    explicit Quad(const cell_type & cell);

    //! Quad Constructor
    /*!
       \brief Constructs a Quad
       \param id Quad identifier.
     */
    Quad(uint32_t id);

    //! Quad Destructor
    /*!
       \brief Destroys the Quad, including its properties
     */
    ~Quad();

    //! Equality operator
    /*!
       \brief Compares if the cells of two quads are the same.
       \param o Another quad
       \return True if the cells of the quads are equal, otherwise false.
     */
    bool operator==(const Quad & o) const;

    //! Inequality operator
    /*!
       \brief Compares if the cells of two quads are different.
       \param o Another quad
       \return True if the cells of the quads are equal, otherwise false.
     */
    bool operator!=(const Quad & o) const;

    cell_type mCell;
};

class WireQuad {
public:
  using cell_type = ophidian::circuit::Netlist::cell_instance_type;
    //! WireQuad Constructor
    /*!
       \brief Constructs a WireQuad
     */
    WireQuad();

    //! WireQuad Constructor
    /*!
       \brief Constructs a WireQuad
       \param cell A circuit cell
     */
    explicit WireQuad(const cell_type & cell);

    //! WireQuad Destructor
    /*!
       \brief Destroys the WireQuad, including its properties
     */
    ~WireQuad();

    std::array<Line, 4> mLines;
    cell_type mCell;
};


#endif // FORMS_H
