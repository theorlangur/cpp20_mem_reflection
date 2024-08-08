#ifndef REFLECTION_FUNCTIONS_H_
#define REFLECTION_FUNCTIONS_H_

#include "reflection_members.h"

namespace refl
{
    //invoke a 'callback' for each field member of 'v'
    template<class T, class CB>
    inline constexpr void for_each_mem(T const& v, CB && callback)
    {
        members<T, refl::info<T>::get_members_count()>::for_each(v, std::forward<CB>(callback));
    }

    //invoke a 'callback' for all field members of 'v'
    //at once
    template<class T>
    inline constexpr auto fetch_all_mem(auto && callback)
    {
        return members<T, refl::info<T>::get_members_count()>::fetch_all(T{}, callback);
    }

    //returns the sum of sizeof() for all member field types
    template<class T>
    inline consteval auto get_all_fields_size()
    {
        return members<T, refl::info<T>::get_members_count()>::all_fields_size();
    }
}

#endif
