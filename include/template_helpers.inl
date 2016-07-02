template <typename F, typename... Args>
void for_each_arg(F&& f, Args&&... args)
{
    // Waiting for C++17, so I don't have to write silly stuff like this
    using expand = bool[];
    (void)expand
    {
        (
            f(std::forward<Args>(args)),
            true
        )...
    };
}

template <typename F, typename Tuple, size_t... I>
decltype(auto) apply_impl(F&& f, Tuple&& t, std::index_sequence<I...>)
{
    return std::forward<F>(f)(std::get<I>(std::forward<Tuple>(t))...);
}

template <typename F, typename Tuple>
decltype(auto) apply_impl(F&& f, Tuple&& t)
{
    return std::forward<F>(f)(std::get<0>(std::forward<Tuple>(t)));
}

template <typename F, typename Tuple, typename = std::enable_if_t<(std::tuple_size<std::decay_t<Tuple>>::value > 1)>>
decltype(auto) apply(F&& f, Tuple&& t)
{
    using Indices =
        std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>;

    return apply_impl(std::forward<F>(f), std::forward<Tuple>(t), Indices{});
}

template <typename F, typename Tuple, typename = std::enable_if_t<!(std::tuple_size<std::decay_t<Tuple>>::value > 1)>, typename = void>
decltype(auto) apply(F&& f, Tuple&& t)
{
    return apply_impl(std::forward<F>(f), std::forward<Tuple>(t));
}

template <typename F, typename TupleT>
void for_tuple(F&& f, TupleT&& tuple)
{
    apply(
        [&f](auto&&... elems) {
            for_each_arg(f,
                         std::forward<decltype(elems)>(elems)...);
        },
        std::forward<TupleT>(tuple));
}

template <typename F>
void for_tuple(F&& /* f */, const std::tuple<>& /* tuple */)
{ /* do nothing */ }

template <bool Test,
    typename F, typename... Args,
    typename>
void call_if(F&& f, Args&&... args)
{
    f(std::forward<Args>(args)...);
}

template <bool Test,
    typename F, typename... Args,
    typename, typename>
void call_if(F&& /* f */, Args&&... /* args */)
{ /* do nothing */ }