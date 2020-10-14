#include <sqlite_orm/sqlite_orm.h>
#include <string>

struct Vocabulary {
    std::string word;
    int count = 0;
};

int main() {
    using namespace sqlite_orm;
    
    auto storage = make_storage({},
                                make_table("vocabulary",
                                           make_column("word", &Vocabulary::word, primary_key()),
                                           make_column("count", &Vocabulary::count, default_value(1))));
    storage.sync_schema();
    
    storage.replace(Vocabulary{"jovial"}, on_conflict().do_nothing());
    return 0;
}
