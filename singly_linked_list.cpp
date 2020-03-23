#include <iostream>

class Singly_linked_list;

struct Signly_link
{
    Signly_link() :next(nullptr)
    {}

    int i;

protected:
    friend class Singly_linked_list;
    Signly_link* next;
};

class Singly_linked_list
{
public:
    Singly_linked_list() :head_(),
        tail_(nullptr)
    {
#ifdef _DEBUG
        head_.next = nil();
        tail_ = nil();
#endif // DEBUG

    }

    Singly_linked_list(const Singly_linked_list&) = delete;
    Singly_linked_list& operator=(const Singly_linked_list&) = delete;

    void put(Signly_link* e)
    {
#ifdef _DEBUG
        assert(e->next == nullptr);
        e->next = nil();
#endif

        if (is_not_nil(tail_))
        {
            tail_->next = e;
            tail_ = e;
        }
        else
        {
            head_.next = e;
            tail_ = e;
        }
    }

    Signly_link* get()
    {
        auto* e = head_.next;
        
        if (is_not_nil(e))
        {
            head_.next = e->next;
            if (is_nil(head_.next))
            {
                tail_ = nil();
            }
#ifdef _DEBUG
            e->next = nullptr;
#endif // _DEBUG

            return e;
        }
        else 
        {
            return nullptr;
        }
    }

private:

#ifndef _DEBUG
    bool is_nil(Signly_link* e) const 
    {
        return (e == nullptr);
    }

    bool is_not_nil(Signly_link* e) const
    {
        return (e != nullptr);
    }

    Signly_link* nil() const
    {
        return nullptr;
    }
#else

    bool is_nil(const Signly_link* e) const
    {
        return (e == &head_);
    }

    bool is_not_nil(const Signly_link* e) const
    {
        return (e != &head_);
    }

    Signly_link* nil()
    {
        return &head_;
    }

#endif

private:
    Signly_link  head_;
    Signly_link* tail_;
};


int main()
{
    Singly_linked_list list;

    Signly_link link[100];

    size_t i = 0;
    for (auto& l : link)
    {
        l.i = ++i;
        list.put(&l);
    }

    i = 0;
    
    auto* e = list.get();
    while (e)
    {
        if (i >= 100000) break;
        ++i;
        list.put(e);
        std::cout <<i << " " <<  e->i << std::endl;
        e = list.get();
    }

    std::cin.ignore();
}