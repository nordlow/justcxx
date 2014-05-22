#include "utils.hpp"
#include "zui/zui.hpp"
#include "zui/sets.hpp"

int main(int argc, char * argv[])
{
    uint w, h;
    handle_main_args_wh(argc, argv, w, h);

    using namespace zui;

    cout << "URGrid: " << sizeof(Obj) << endl;

    Rect r = Rect(0, 0, w, h * 0.05);
    engine_init(r);
    engine_add_default_view(w, h, 1);
    engine_mainloop();
    engine_clear();

    return 0;
}
