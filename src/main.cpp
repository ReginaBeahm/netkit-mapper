#include "App.h"

int main(int argc, char** argv) {
    Glib::RefPtr<App> app = App::create();

    return app->run(argc, argv);
}