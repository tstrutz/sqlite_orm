#pragma once

#include "optional_container.h"
#include "select_constraints.h"
#include "conditions.h"

namespace sqlite_orm {

namespace internal {

template<class W, class ...Cols>
struct on_coflict_details {
    columns_t<Cols...> columns;
    optional_container<W> where_expression;
};

template<class D, class U>
struct on_conflict_t {
    optional_container<D> details;
    optional_container<U> operation;
};

template<class T, class... Wargs>
struct update_all_t;
    
template<class D>
struct on_conflict_intermediate_t {
    optional_container<D> details;
    
    on_conflict_t<D, void> do_nothing() {
        return {std::move(this->details)};
    }
    
    template<class... Args, class... Wargs, class U = update_all_t<set_t<Args...>, Wargs...>>
    on_conflict_t<D, U> do_update(internal::set_t<Args...> set, Wargs... wh) {
        using update_statement_t = update_all_t<set_t<Args...>, Wargs...>;
        using args_tuple = std::tuple<Wargs...>;
        internal::validate_conditions<args_tuple>();
        args_tuple conditions{std::forward<Wargs>(wh)...};
        update_statement_t update_statement{std::move(set), move(conditions)};
        return {std::move(this->details), std::move(update_statement)};
    }
};
}

inline internal::on_conflict_intermediate_t<void> on_conflict() {
    return {};
}

template<class ...Cols>
internal::on_conflict_intermediate_t<internal::on_coflict_details<void, Cols...>> on_conflict(internal::columns_t<Cols...> columns) {
    return {std::move(columns)};
}

template<class C, class ...Cols>
internal::on_conflict_intermediate_t<internal::on_coflict_details<conditions::where_t<C>, Cols...>> on_conflict(internal::columns_t<Cols...> columns, conditions::where_t<C> wh) {
    return {std::move(columns), std::move(wh)};
}

}
