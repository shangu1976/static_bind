#include <iostream>
#include <type_traits>

template <typename T>
class Static_bind;

template<typename R, typename ...Args>
class Static_bind<R(Args...)>
{
public:
    Static_bind() : obj_(nullptr),
        call_(nullptr),
        destroy_(nullptr)
    {}

    ~Static_bind()
    {
        reset();
    }

    Static_bind(const Static_bind&) = delete;
    Static_bind operator=(const Static_bind&) = delete;

    template<R(* func)(Args...)>
    void bind() 
    {
        reset();

        obj_ = nullptr;
        
        call_ = [](void_ptr, Args ... args)
        {
            return func(std::forward<Args>(args) ...);
        };
        
        destroy_ = nullptr;
    }

    template<typename T, R(T::*mem_fn)(Args...)>
    void bind(T* obj) 
    {
        reset();

        obj_ = reinterpret_cast<void_ptr>(obj);

        call_ = [](void_ptr ptr, Args ... args)
        {
            return (reinterpret_cast<T*>(ptr)->*mem_fn)(std::forward<Args>(args) ...);
        };
        
        destroy_ = nullptr;
    }

    struct const_mem_fn_t {};

    template<typename T, R(T::*mem_fn)(Args...) const>
    void bind(const T* obj, const_mem_fn_t) 
    {
        reset();

        obj_ = reinterpret_cast<void_ptr>(const_cast<T*>(obj));

        call_ = [](void_ptr ptr, Args ... args)
        {
            return (reinterpret_cast<const T*>(ptr)->*mem_fn)(std::forward<Args>(args) ...);
        };

        destroy_ = nullptr;
    }

    template<typename F>
    typename std::enable_if< std::is_empty<typename std::decay<F>::type>::value, void>::type
        bind(F&& functor) 
    {
        using functor_type = typename std::decay<F>::type;

        reset();

        obj_ = reinterpret_cast<void_ptr>(&functor);
        
        call_ = [](void_ptr ptr, Args ... args)
        {
            return (*reinterpret_cast<functor_type*>(ptr))(std::forward<Args>(args) ...);
        };

        destroy_ = nullptr;
    }

    template<typename F>
    typename std::enable_if< !std::is_empty<typename std::decay<F>::type>::value, void>::type
        bind(F&& functor)
    {
        using functor_type = typename std::decay<F>::type;

        reset();

        obj_ = reinterpret_cast<void_ptr>(new functor_type(std::move(functor)));

        call_ = [](void_ptr ptr, Args ... args)
        {
            return (*reinterpret_cast<functor_type*>(ptr))(std::forward<Args>(args) ...);
        };

        destroy_ = [](void_ptr ptr)
        {
            delete reinterpret_cast<functor_type*>(ptr);
        };
    }

    operator bool() const
    {
        return call_ != nullptr;
    }

    R operator()(Args ... args) const
    {
        return call_(obj_, std::forward<Args>(args)...);
    }

    void reset()
    {
        if (call_ != nullptr)
        {
            if (destroy_ != nullptr)
            {
                destroy_(obj_);
            }

            obj_ = nullptr;
            call_ = nullptr;
            destroy_ = nullptr;
        }
    }

private:
    using void_ptr = void*;
    using call = R(*)(void_ptr, Args...);
    using destroy = void(*)(void_ptr);

    void_ptr obj_;
    call call_;
    destroy destroy_;
};

void foo() {
    std::cout << __FUNCTION__ << std::endl;
}

struct Foo
{
    static void sf() { std::cout << __FUNCTION__ << std::endl; };
    void f() { std::cout << __FUNCTION__ << std::endl; };
    void f()const { std::cout << __FUNCTION__ << std::endl; }
    void f1() { std::cout << __FUNCTION__ << std::endl; }
};

using SB = Static_bind<void()>;

int main()
{
    SB sb;

    sb.bind<&foo>();
    sb();
    sb.bind<&Foo::sf>();
    sb();

    Foo foo;
    sb.bind<Foo, &Foo::f1>(&foo);
    sb();
    sb.bind<Foo, &Foo::f>(&foo);
    sb();

    sb.bind<Foo, &Foo::f>(&foo, SB::const_mem_fn_t());
    sb();

    sb.bind([]() { std::cout << __FUNCTION__ << std::endl; });
    sb();

    struct F
    {
        ~F()
        {
            std::cout << __FUNCTION__ << std::endl;
        }
    };
    F f;
    sb.bind([f]() { std::cout << __FUNCTION__ << std::endl; });
    sb();

    sb.reset();

    std::cout << "Hello World!\n";
}
