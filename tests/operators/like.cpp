#include <sqlite_orm/sqlite_orm.h>
#include <catch2/catch.hpp>

using namespace sqlite_orm;

TEST_CASE("Like operator") {
    struct User {
        int id = 0;
        std::string name;
    };
    struct Pattern {
        std::string value;
    };
    
    struct Vocabulary {
        std::string text;
        int count = 0;
    };

    auto storage = make_storage("",
                                make_table("users",
                                           make_column("id", &User::id, autoincrement(), primary_key()),
                                           make_column("name", &User::name)),
                                make_table("patterns", make_column("value", &Pattern::value)));
    storage.sync_schema();

    //  INSERT INTO users('name')
    //  VALUES('Sia')
    //  ON CONFLICT DO NOTHING
    storage.insert(User{0, "Sia"}/*, on_conflict().do_nothing())*/);
    
    //  INSERT INTO users('name')
    //  VALUES('Sia')
    //  ON CONFLICT ('word')
    //  DO UPDATE SET count = count + 1
    //storage.insert(User{0, "Sia"}, on_conflict(&Vocabulary::word).do_(update_all(set(c(&Vocabulary::count) = c(&Vocabulary::count) + 1))));
    
    //  INSERT INTO users('name')
    //  VALUES('Sia')
    //  ON CONFLICT ('word')
    //  WHERE count IS NOT NULL
    //  DO UPDATE SET count = count + 1
    //storage.insert(User{0, "Sia"}, on_conflict(&Vocabulary::word, where(is_not_null(&Vocabulary::count))).do_(update_all(set(c(&Vocabulary::count) = c(&Vocabulary::count) + 1))));
    
    
    //  INSERT INTO users('name')
    //  VALUES('Sia')
    //  ON CONFLICT
    //  DO UPDATE SET count = count + 1
    //storage.insert(User{0, "Sia"}, on_conflict().do_(update_all(set(c(&Vocabulary::count) = c(&Vocabulary::count) + 1))));
    storage.insert(User{0, "Stark"});
    storage.insert(User{0, "Index"});

    auto whereCondition = where(like(&User::name, "S%"));
    {
        auto users = storage.get_all<User>(whereCondition);
        REQUIRE(users.size() == 2);
    }
    {
        auto rows = storage.select(&User::id, whereCondition);
        REQUIRE(rows.size() == 2);
    }
    {
        auto rows = storage.select(like("ototo", "ot_to"));
        REQUIRE(rows.size() == 1);
        REQUIRE(rows.front() == true);
    }
    {
        auto rows = storage.select(not like("ototo", "ot_to"));
        REQUIRE(rows.size() == 1);
        REQUIRE(rows.front() == false);
    }
    {
        auto rows = storage.select(like(&User::name, "S%a"));
        REQUIRE(rows.size() == 3);
        REQUIRE(count_if(rows.begin(), rows.end(), [](bool arg) {
                    return arg == true;
                }) == 1);
    }

    storage.insert(Pattern{"o%o"});
    {
        auto rows = storage.select(like("ototo", &Pattern::value));
        REQUIRE(rows.size() == 1);
        REQUIRE(rows.front() == true);
    }
    {
        auto rows = storage.select(like("aaa", &Pattern::value));
        REQUIRE(rows.size() == 1);
        REQUIRE(rows.front() == false);
    }
}
