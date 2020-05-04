
#ifndef libunittest__utility_hxx_
#define libunittest__utility_hxx_

namespace utility {

template< typename Class >
class Intrusive_instance_list {
protected:
  Intrusive_instance_list()
  {
    if (!first_)
      first_ = this;

    prev_ = last_;
    last_ = this;

    if (prev_)
      prev_->next_ = this;

    ++instance_count_;
  }

  Intrusive_instance_list(Intrusive_instance_list const&) = delete;
  Intrusive_instance_list(Intrusive_instance_list&&) = delete;

  ~Intrusive_instance_list() noexcept
  {
    if (first_ == this)
      first_ = next_;

    if (last_ == this)
      last_ = prev_;

    if (prev_)
      prev_->next_ = next_;

    if (next_)
      next_->prev_ = prev_;

    --instance_count_;
  }

  Intrusive_instance_list&
  operator=(Intrusive_instance_list const&) = delete;
  Intrusive_instance_list&
  operator=(Intrusive_instance_list&&) = delete;

  Class*
  previous()
  {
    return static_cast< Class* >(prev_);
  }

  Class*
  next()
  {
    return static_cast< Class* >(next_);
  }

  static Class*
  first()
  {
    return static_cast< Class* >(first_);
  }

  static Class*
  last()
  {
    return static_cast< Class* >(last_);
  }

  static std::size_t
  count()
  {
    return instance_count_;
  }

private:
  Intrusive_instance_list< Class >* prev_;
  Intrusive_instance_list< Class >* next_;

  static Intrusive_instance_list< Class >* first_;
  static Intrusive_instance_list< Class >* last_;

  static std::size_t instance_count_;
};

template< typename Class >
Intrusive_instance_list< Class >* Intrusive_instance_list< Class >::first_{
  nullptr
};

template< typename Class >
Intrusive_instance_list< Class >* Intrusive_instance_list< Class >::last_{
  nullptr
};

template< typename Class >
std::size_t Intrusive_instance_list< Class >::instance_count_{ 0 };

template< typename T >
class Current {
public:
  explicit Current(T* current)
  {
    current_ = current;
  }

  ~Current()
  {
    current_ = nullptr;
  }

  static T*
  get()
  {
    return current_;
  }

private:
  static T* current_;
};

template< typename T >
T* Current< T >::current_{ nullptr };

struct Point_exception {
  std::string id;
};

} // namespace utility

#endif
