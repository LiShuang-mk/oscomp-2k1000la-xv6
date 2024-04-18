#include "typeinfo.hh"
#include "common.hh"
// #include <bits/stdc++.h>

#define NULL 0

extern "C" void __cxa_pure_virtual(void)
{
    // panic("pure virtual called\n");
}
int strcmp(const char *, const char *){}
namespace __cxxabiv1 {

namespace {

using namespace std;
using namespace __cxxabiv1;

// Initial part of a vtable, this structure is used with offsetof, so we don't
// have to keep alignments consistent manually.
struct vtable_prefix 
{
  // Offset to most derived object.
  ptrdiff_t whole_object;

  // Additional padding if necessary.
#ifdef _GLIBCXX_VTABLE_PADDING
  ptrdiff_t padding1;               
#endif

  // Pointer to most derived type_info.
  const __class_type_info *whole_type;  

  // Additional padding if necessary.
#ifdef _GLIBCXX_VTABLE_PADDING
  ptrdiff_t padding2;               
#endif

  // What a class's vptr points to.
  const void *origin;               
};

template <typename T>
inline const T *
adjust_pointer (const void *base, ptrdiff_t offset)
{
  return reinterpret_cast <const T *>
    (reinterpret_cast <const char *> (base) + offset);
}

// ADDR is a pointer to an object.  Convert it to a pointer to a base,
// using OFFSET. IS_VIRTUAL is true, if we are getting a virtual base.
inline void const *
convert_to_base (void const *addr, bool is_virtual, ptrdiff_t offset)
{
  if (is_virtual)
    {
      const void *vtable = *static_cast <const void *const *> (addr);
      
      offset = *adjust_pointer<ptrdiff_t> (vtable, offset);
    }

  return adjust_pointer<void> (addr, offset);
}

// some predicate functions for __class_type_info::__sub_kind
inline bool contained_p (__class_type_info::__sub_kind access_path)
{
  return access_path >= __class_type_info::__contained_mask;
}
inline bool public_p (__class_type_info::__sub_kind access_path)
{
  return access_path & __class_type_info::__contained_public_mask;
}
inline bool virtual_p (__class_type_info::__sub_kind access_path)
{
  return (access_path & __class_type_info::__contained_virtual_mask);
}
inline bool contained_public_p (__class_type_info::__sub_kind access_path)
{
  return ((access_path & __class_type_info::__contained_public)
          == __class_type_info::__contained_public);
}
inline bool contained_nonpublic_p (__class_type_info::__sub_kind access_path)
{
  return ((access_path & __class_type_info::__contained_public)
          == __class_type_info::__contained_mask);
}
inline bool contained_nonvirtual_p (__class_type_info::__sub_kind access_path)
{
  return ((access_path & (__class_type_info::__contained_mask
                          | __class_type_info::__contained_virtual_mask))
          == __class_type_info::__contained_mask);
}

static const __class_type_info *const nonvirtual_base_type =
    static_cast <const __class_type_info *> (0) + 1;

} // namespace

// __upcast_result is used to hold information during traversal of a class
// hierarchy when catch matching.
struct __class_type_info::__upcast_result
{
  const void *dst_ptr;        // pointer to caught object
  __sub_kind part2dst;        // path from current base to target
  int src_details;            // hints about the source type hierarchy
  const __class_type_info *base_type; // where we found the target,
                              // if in vbase the __class_type_info of vbase
                              // if a non-virtual base then 1
                              // else NULL
  __upcast_result (int d)
    :dst_ptr (NULL), part2dst (__unknown), src_details (d), base_type (NULL)
    {}
};

// __dyncast_result is used to hold information during traversal of a class
// hierarchy when dynamic casting.
struct __class_type_info::__dyncast_result
{
  const void *dst_ptr;        // pointer to target object or NULL
  __sub_kind whole2dst;       // path from most derived object to target
  __sub_kind whole2src;       // path from most derived object to sub object
  __sub_kind dst2src;         // path from target to sub object
  int whole_details;          // details of the whole class hierarchy
  
  __dyncast_result (int details_ = __vmi_class_type_info::__flags_unknown_mask)
    :dst_ptr (NULL), whole2dst (__unknown),
     whole2src (__unknown), dst2src (__unknown),
     whole_details (details_)
    {}

protected:
  __dyncast_result(const __dyncast_result&);
  
  __dyncast_result&
  operator=(const __dyncast_result&);
};

inline __class_type_info::__sub_kind __class_type_info::
__find_public_src (ptrdiff_t src2dst,
                   const void *obj_ptr,
                   const __class_type_info *src_type,
                   const void *src_ptr) const
{
  if (src2dst >= 0)
    return adjust_pointer <void> (obj_ptr, src2dst) == src_ptr
            ? __contained_public : __not_contained;
  if (src2dst == -2)
    return __not_contained;
  return __do_find_public_src (src2dst, obj_ptr, src_type, src_ptr);
}

}
void operator delete(void*, unsigned long){}
namespace __cxxabiv1 {

__si_class_type_info::
~__si_class_type_info ()
{}

__class_type_info::__sub_kind __si_class_type_info::
__do_find_public_src (ptrdiff_t src2dst,
                      const void *obj_ptr,
                      const __class_type_info *src_type,
                      const void *src_ptr) const
{
  if (src_ptr == obj_ptr && *this == *src_type)
    return __contained_public;
  return __base_type->__do_find_public_src (src2dst, obj_ptr, src_type, src_ptr);
}

bool __si_class_type_info::
__do_dyncast (ptrdiff_t src2dst,
              __sub_kind access_path,
              const __class_type_info *dst_type,
              const void *obj_ptr,
              const __class_type_info *src_type,
              const void *src_ptr,
              __dyncast_result &__restrict result) const
{
  if (*this == *dst_type)
    {
      result.dst_ptr = obj_ptr;
      result.whole2dst = access_path;
      if (src2dst >= 0)
        result.dst2src = adjust_pointer <void> (obj_ptr, src2dst) == src_ptr
              ? __contained_public : __not_contained;
      else if (src2dst == -2)
        result.dst2src = __not_contained;
      return false;
    }
  if (obj_ptr == src_ptr && *this == *src_type)
    {
      // The src object we started from. Indicate how we are accessible from
      // the most derived object.
      result.whole2src = access_path;
      return false;
    }
  return __base_type->__do_dyncast (src2dst, access_path, dst_type, obj_ptr,
                             src_type, src_ptr, result);
}

bool __si_class_type_info::
__do_upcast (const __class_type_info *dst, const void *obj_ptr,
             __upcast_result &__restrict result) const
{
  if (__class_type_info::__do_upcast (dst, obj_ptr, result))
    return true;
  
  return __base_type->__do_upcast (dst, obj_ptr, result);
}

}

namespace __cxxabiv1 {

__class_type_info::
~__class_type_info ()
{}

bool __class_type_info::
__do_catch (const type_info *thr_type,
            void **thr_obj,
            unsigned outer) const
{
  if (*this == *thr_type)
    return true;
  if (outer >= 4)
    // Neither `A' nor `A *'.
    return false;
  return thr_type->__do_upcast (this, thr_obj);
}

bool __class_type_info::
__do_upcast (const __class_type_info *dst_type,
             void **obj_ptr) const
{
  __upcast_result result (__vmi_class_type_info::__flags_unknown_mask);
  
  __do_upcast (dst_type, *obj_ptr, result);
  if (!contained_public_p (result.part2dst))
    return false;
  *obj_ptr = const_cast <void *> (result.dst_ptr);
  return true;
}

__class_type_info::__sub_kind __class_type_info::
__do_find_public_src (ptrdiff_t,
                      const void *obj_ptr,
                      const __class_type_info *,
                      const void *src_ptr) const
{
  if (src_ptr == obj_ptr)
    // Must be our type, as the pointers match.
    return __contained_public;
  return __not_contained;
}

bool __class_type_info::
__do_dyncast (ptrdiff_t,
              __sub_kind access_path,
              const __class_type_info *dst_type,
              const void *obj_ptr,
              const __class_type_info *src_type,
              const void *src_ptr,
              __dyncast_result &__restrict result) const
{
  if (obj_ptr == src_ptr && *this == *src_type)
    {
      // The src object we started from. Indicate how we are accessible from
      // the most derived object.
      result.whole2src = access_path;
      return false;
    }
  if (*this == *dst_type)
    {
      result.dst_ptr = obj_ptr;
      result.whole2dst = access_path;
      result.dst2src = __not_contained;
      return false;
    }
  return false;
}

bool __class_type_info::
__do_upcast (const __class_type_info *dst, const void *obj,
             __upcast_result &__restrict result) const
{
  if (*this == *dst)
    {
      result.dst_ptr = obj;
      result.base_type = nonvirtual_base_type;
      result.part2dst = __contained_public;
      return true;
    }
  return false;
}

}


#include <bits/c++config.h>
#include <cstddef>

std::type_info::
~type_info ()
{ }

#if !__GXX_TYPEINFO_EQUALITY_INLINE

#if __cplusplus > 202002L
# error "this file must be compiled with C++20 or older to define operator=="
#endif

// We can't rely on common symbols being shared between shared objects.
bool std::type_info::
operator== (const std::type_info& arg) const _GLIBCXX_NOEXCEPT
{
    return true;
}

bool
std::type_info::__equal (const std::type_info& arg) const _GLIBCXX_NOEXCEPT
__attribute__((alias("_ZNKSt9type_infoeqERKS_")));
#endif

namespace std {

// return true if this is a type_info for a pointer type
bool type_info::
__is_pointer_p () const
{
  return false;
}

// return true if this is a type_info for a function type
bool type_info::
__is_function_p () const
{
  return false;
}

// try and catch a thrown object.
bool type_info::
__do_catch (const type_info *thr_type, void **, unsigned) const
{
  return *this == *thr_type;
}

// upcast from this type to the target. __class_type_info will override
bool type_info::
__do_upcast (const __cxxabiv1::__class_type_info *, void **) const
{
  return false;
}

}

namespace __cxxabiv1 {


// this is the external interface to the dynamic cast machinery
/* sub: source address to be adjusted; nonnull, and since the
 *      source object is polymorphic, *(void**)sub is a virtual pointer.
 * src: static type of the source object.
 * dst: destination type (the "T" in "dynamic_cast<T>(v)").
 * src2dst_offset: a static hint about the location of the
 *    source subobject with respect to the complete object;
 *    special negative values are:
 *       -1: no hint
 *       -2: src is not a public base of dst
 *       -3: src is a multiple public base type but never a
 *           virtual base type
 *    otherwise, the src type is a unique public nonvirtual
 *    base type of dst at offset src2dst_offset from the
 *    origin of dst.  */
extern "C" void *
__dynamic_cast (const void *src_ptr,    // object started from
                const __class_type_info *src_type, // type of the starting object
                const __class_type_info *dst_type, // desired target type
                ptrdiff_t src2dst) // how src and dst are related
  {
  if (__builtin_expect(!src_ptr, 0))
    return NULL; // Handle precondition violations gracefully.

  const void *vtable = *static_cast <const void *const *> (src_ptr);
  const vtable_prefix *prefix =
    (adjust_pointer <vtable_prefix>
     (vtable,  -ptrdiff_t (offsetof (vtable_prefix, origin))));
  const void *whole_ptr =
      adjust_pointer <void> (src_ptr, prefix->whole_object);
  const __class_type_info *whole_type = prefix->whole_type;
  __class_type_info::__dyncast_result result;

  // If the whole object vptr doesn't refer to the whole object type, we're
  // in the middle of constructing a primary base, and src is a separate
  // base.  This has undefined behavior and we can't find anything outside
  // of the base we're actually constructing, so fail now rather than
  // segfault later trying to use a vbase offset that doesn't exist.
  const void *whole_vtable = *static_cast <const void *const *> (whole_ptr);
  const vtable_prefix *whole_prefix =
    (adjust_pointer <vtable_prefix>
     (whole_vtable, -ptrdiff_t (offsetof (vtable_prefix, origin))));
  if (whole_prefix->whole_type != whole_type)
    return NULL;

  // Avoid virtual function call in the simple success case.
  if (src2dst >= 0
      && src2dst == -prefix->whole_object
      && *whole_type == *dst_type)
    return const_cast <void *> (whole_ptr);

  whole_type->__do_dyncast (src2dst, __class_type_info::__contained_public,
                            dst_type, whole_ptr, src_type, src_ptr, result);
  if (!result.dst_ptr)
    return NULL;
  if (contained_public_p (result.dst2src))
    // Src is known to be a public base of dst.
    return const_cast <void *> (result.dst_ptr);
  if (contained_public_p (__class_type_info::__sub_kind
			  (result.whole2src & result.whole2dst)))
    // Both src and dst are known to be public bases of whole. Found a valid
    // cross cast.
    return const_cast <void *> (result.dst_ptr);
  if (contained_nonvirtual_p (result.whole2src))
    // Src is known to be a non-public nonvirtual base of whole, and not a
    // base of dst. Found an invalid cross cast, which cannot also be a down
    // cast
    return NULL;
  if (result.dst2src == __class_type_info::__unknown)
    result.dst2src = dst_type->__find_public_src (src2dst, result.dst_ptr,
                                                  src_type, src_ptr);
  if (contained_public_p (result.dst2src))
    // Found a valid down cast
    return const_cast <void *> (result.dst_ptr);
  // Must be an invalid down cast, or the cross cast wasn't bettered
  return NULL;
}

}
