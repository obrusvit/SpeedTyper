#ifndef SQLPP_SCORE_TABLE
#define SQLPP_SCORE_TABLE

#include <sqlpp11/char_sequence.h>
#include <sqlpp11/column_types.h>
#include <sqlpp11/data_types/day_point/data_type.h>
#include <sqlpp11/data_types/integral/data_type.h>
#include <sqlpp11/data_types/no_value/data_type.h>
#include <sqlpp11/data_types/time_of_day/data_type.h>
#include <sqlpp11/data_types/time_point/data_type.h>
#include <sqlpp11/table.h>
#include <sqlpp11/type_traits.h>

namespace TabScore_ {
struct id {
    struct _alias_t {
        static constexpr const char _literal[] = "id";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T id;
            T& operator()() { return id; }
            const T& operator()() const { return id; }
        };
    };
    using _traits = ::sqlpp::make_traits<::sqlpp::bigint, ::sqlpp::tag::must_not_insert,
                                         ::sqlpp::tag::must_not_update, ::sqlpp::tag::can_be_null>;
};

struct words_correct {
    struct _alias_t {
        static constexpr const char _literal[] = "words_correct";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T words_correct;
            T& operator()() { return words_correct; }
            const T& operator()() const { return words_correct; }
        };
    };
    using _traits = ::sqlpp::make_traits<::sqlpp::integer>;
};

struct words_bad {
    struct _alias_t {
        static constexpr const char _literal[] = "words_bad";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T words_bad;
            T& operator()() { return words_bad; }
            const T& operator()() const { return words_bad; }
        };
    };
    using _traits = ::sqlpp::make_traits<::sqlpp::integer>;
};

struct chars_correct {
    struct _alias_t {
        static constexpr const char _literal[] = "chars_correct";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T chars_correct;
            T& operator()() { return chars_correct; }
            const T& operator()() const { return chars_correct; }
        };
    };
    using _traits = ::sqlpp::make_traits<::sqlpp::integer>;
};

struct chars_bad {
    struct _alias_t {
        static constexpr const char _literal[] = "chars_bad";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T chars_bad;
            T& operator()() { return chars_bad; }
            const T& operator()() const { return chars_bad; }
        };
    };
    using _traits = ::sqlpp::make_traits<::sqlpp::integer>;
};

struct backspaces {
    struct _alias_t {
        static constexpr const char _literal[] = "backspaces";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T backspaces;
            T& operator()() { return backspaces; }
            const T& operator()() const { return backspaces; }
        };
    };
    using _traits = ::sqlpp::make_traits<::sqlpp::integer, ::sqlpp::tag::can_be_null>;
};
struct spaces {
    struct _alias_t {
        static constexpr const char _literal[] = "spaces";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T spaces;
            T& operator()() { return spaces; }
            const T& operator()() const { return spaces; }
        };
    };
    using _traits = ::sqlpp::make_traits<::sqlpp::integer, ::sqlpp::tag::can_be_null>;
};

struct key_presses {
    struct _alias_t {
        static constexpr const char _literal[] = "key_presses";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T key_presses;
            T& operator()() { return key_presses; }
            const T& operator()() const { return key_presses; }
        };
    };
    using _traits = ::sqlpp::make_traits<::sqlpp::integer, ::sqlpp::tag::can_be_null>;
};

struct wpm {
    struct _alias_t {
        static constexpr const char _literal[] = "wpm";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T wpm;
            T& operator()() { return wpm; }
            const T& operator()() const { return wpm; }
        };
    };
    using _traits = ::sqlpp::make_traits<::sqlpp::integer>;
};

struct cpm {
    struct _alias_t {
        static constexpr const char _literal[] = "cpm";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T cpm;
            T& operator()() { return cpm; }
            const T& operator()() const { return cpm; }
        };
    };
    using _traits = ::sqlpp::make_traits<::sqlpp::integer, ::sqlpp::tag::can_be_null>;
};

struct test_duration {
    struct _alias_t {
        static constexpr const char _literal[] = "test_duration";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T test_duration;
            T& operator()() { return test_duration; }
            const T& operator()() const { return test_duration; }
        };
    };
    using _traits = ::sqlpp::make_traits<::sqlpp::integer>;
};

struct test_datetime {
    struct _alias_t {
        static constexpr const char _literal[] = "test_datetime";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T test_datetime;
            T& operator()() { return test_datetime; }
            const T& operator()() const { return test_datetime; }
        };
    };
    using _traits = ::sqlpp::make_traits<::sqlpp::time_point, ::sqlpp::tag::must_not_update>;
};

} // namespace TabScore_

struct TabScore
    : sqlpp::table_t<TabScore, TabScore_::id, TabScore_::words_correct, TabScore_::words_bad,
                     TabScore_::chars_correct, TabScore_::chars_bad, TabScore_::backspaces,
                     TabScore_::spaces, TabScore_::key_presses, TabScore_::wpm, TabScore_::cpm,
                     TabScore_::test_duration, TabScore_::test_datetime> {
    using _value_type = sqlpp::no_value_t;
    struct _alias_t {
        static constexpr const char _literal[] = "tab_score";
        using _name_t = sqlpp::make_char_sequence<sizeof(_literal), _literal>;
        template <typename T> struct _member_t {
            T tabScore;
            T& operator()() { return tabScore; }
            const T& operator()() const { return tabScore; }
        };
    };
};

#endif /* ifndef SQLPP_SCORE_TABLE */
