#ifndef OPHIDIAN_ENTITY_SYSTEM_ENTITY_SYSTEM_H
#define OPHIDIAN_ENTITY_SYSTEM_ENTITY_SYSTEM_H

#include <memory>
#include <array>

namespace ophidian {
namespace entity_system {

class abstract_property;
class entity_system
{
public:

    class entity {
    public:
        friend class entity_system;

        entity();
        entity(const entity& o);
        entity& operator=(const entity&o);

        bool operator<(const entity&o) const;
        bool operator==(const entity&o) const;
        bool operator!=(const entity&o) const;
        std::size_t id() const {
            return m_id;
        }

        static entity null();
        static std::size_t invalid();

    private:
        std::size_t m_id;
        explicit entity(std::size_t id) : m_id(id) {};
    };

    class notifier_ {
    public:

        notifier_();
        ~notifier_();

        bool has_property(abstract_property &prop) const;
        std::size_t properties_size() const;

        void attach(abstract_property& prop);
        void dettach(abstract_property& prop);
        void create(entity en);
        void destroy(entity en);
        void clear();

    private:
        struct impl;
        std::unique_ptr<impl> this_;
    };

    class const_iterator : public std::iterator<std::random_access_iterator_tag, entity, std::ptrdiff_t> {
    public:
        friend class entity_system;
        const_iterator();
        ~const_iterator();
        const_iterator(const const_iterator & o);
        const_iterator& operator=(const const_iterator & o);
        const const_iterator& operator++(void);
        difference_type operator-(const const_iterator & o) const;
        const entity& operator*() const;
        bool operator==(const const_iterator & it) const;
        bool operator!=(const const_iterator & it) const;
    private:
        struct impl;
        const_iterator(const impl & i);
        std::unique_ptr<impl> this_;
    };

    entity_system();
    virtual ~entity_system();

    std::size_t size() const;
    std::size_t lookup(entity e) const;
    bool valid(entity e) const;
    notifier_ * notifier() const;
    bool has_property(abstract_property &prop) const;
    bool operator==(const entity_system& other) const;
    const const_iterator begin() const;
    const const_iterator end() const;
    std::size_t properties_size() const;

    entity create();
    void destroy(entity e);
    void clear();

    static const entity_system& null();

private:
    struct impl;
    std::unique_ptr<impl> this_;
};

// helper functs
template <int NumberOfElements>
std::array<entity_system::entity, NumberOfElements> make_entities(entity_system& e) {
    std::array<entity_system::entity, NumberOfElements> elements;
    for(auto & el : elements)
        el = e.create();
    return elements;
}

}
}

#endif // OPHIDIAN_ENTITY_SYSTEM_ENTITY_SYSTEM_H
