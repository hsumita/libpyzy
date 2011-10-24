#include "PyZyPhoneticContext.h"

#include "PyZyFullPinyinContext.h"
#include "PyZyDoublePinyinContext.h"
#include "PyZyBopomofoContext.h"
#include "PyZyDatabase.h"

namespace PyZy {

void
InputContext::init ()
{
    init ("libpyzy");
}

void
InputContext::init (const std::string & user_data_dir)
{
    Database::init (user_data_dir);
}

void
InputContext::finalize ()
{
    Database::finalize ();
}


InputContext *
InputContext::create (InputContext::InputType type,
                      Config & config,
                      InputContext::Observer * observer) {
    switch (type) {
    case FULL_PINYIN:
        return new FullPinyinContext (config, observer);
    case DOUBLE_PINYIN:
        return new DoublePinyinContext (config, observer);
    case BOPOMOFO:
        return new BopomofoContext (config, observer);
    default:
        g_warning ("unknown context type.\n");
        return NULL;
    }
}

}  // namespace PyZy
