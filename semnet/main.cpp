#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <future>

#include <semnet/preg.hpp>
#include <semnet/ob.hpp>
#include <semnet/file.hpp>
#include <semnet/regfile.hpp>
#include <semnet/dir.hpp>
#include <semnet/know_all.hpp>
#include <semnet/lit.hpp>
#include <semnet/alt.hpp>
#include <semnet/seq.hpp>
#include <semnet/sit.hpp>
#include <semnet/rep.hpp>
#include <semnet/iter.hpp>
#include <semnet/filetype.hpp>

#include "../substr_match.h"
#include "../readline_utils.h"
#include "../timing.h"
#include "../stdio_x.h"
#include "../csc.hpp"
#include "../enforce.hpp"
#include "../substr_match.hpp"
#include "../strace/strace.h"
#include "../show.hpp"
#include "../bix.hpp"
// #include "../ntags/main.hpp"

using std::cout;
using std::endl;

int semnet_main();

// int foo(int argc, char * argv[])
// {
//     return ntags_main(argc, argv); // just check linkage
// }

int main(int argc, char * argv[])
{
    //auto ok = parseFile (const char *const fileName)

    auto strace = std::async(strace_main, argc, argv); // spawn in new thread
    strace.get();

    using namespace semnet;
    using namespace semnet::filesystem;
    using namespace semnet::patterns;
    using namespace semnet::patterns::gen;

    learn_all();

    {
        auto ls = Dir::load_path("/bin/ls");
        if (auto reg_ls = dynamic_cast<RegFile*>(ls)) {
            auto hit = gp_ELF_ftype->match(reg_ls, bir::full(), FileType::PRECOG_CONTENTS);
            enforce(hit);
        }
    }

    {
        const csc s("alpha") ;
        auto hit = C::gp_ID->match_at(s);
        enforce_eq(hit.bitsize(), bix(8*s.size()));
    }
    {
        const csc s("alpha1") ;
        auto hit = C::gp_ID->match_at(s);
        enforce_eq(hit.bitsize(), bix(8*s.size()));
    }
    {
        const csc s("1a") ;
        auto hit = C::gp_ID->match_at(s);
        enforce(not hit);
    }

    {
        auto hit = C::gp_ID->match_at("$");
        enforce_eq(hit.get_offset(), 0_bix);
        enforce_eq(hit.bitlength(), 0_bix);
    }

    {
        auto hit = C::gp_ID->match_in(csc("ab"), bir(0_bix, 1_bix));
        hit.show(cout); cout << endl;
    }

    return 0;

    {
        auto hit = C::gp_ID->match_at("$alpha");
        hit.show(cout); cout << endl;
    }

    {
        auto hit = C::gp_ID->match_at("alpha beta");
        hit.show(cout); cout << endl;
        enforce(hit);
    }

    // pattern stuff
    auto some = seq(bob(), lit("alpha_"), lit("beta_"), rep(2, lit("gamma_")), eob());
    enforce(some->is_constant());
    auto somec = some->constant();
    enforce_eq(somec, "alpha_beta_gamma_gamma_");

    // load standard files and dirs
    auto home = Dir::home();
    auto etc = Dir::load_path("/etc");

    // load images
    {
        auto images = home->load_to("Images");
        if (auto dir = dynamic_cast<Dir*>(images)) { // if a directory
            dir->load(true, true); // load and scan
            cout << dir->path() << "::get_tree_csize: " << dir->get_tree_csize() << endl;
        }
    }

    {
        auto tmp = home->dig_to("tmp"); // create if not present and go there
        if (auto dir = dynamic_cast<Dir*>(tmp)) { // if a directory
            etc->copy_into(dir);                  // copy stuff from etc
            // \todo: We need some way of asserting that copy operation has completed before continuing here. Maybe using a future?
            dir->load(true, true);                // load and scan it
            cout << dir->path() << "::get_tree_csize: " << dir->get_tree_csize() << endl;
        }
    }

    if (true) {
        auto musik = home->load_to("Musik");
        if (auto dir = dynamic_cast<Dir*>(musik)) {            // if a directory
            dir->load(true, true); // load and scan it
            cout << dir->path() << "::get_tree_csize: " << dir->get_tree_csize() << endl;
        }
    }

    if (true) {
        auto include = File::load_path("/usr/include");
        if (auto dir = dynamic_cast<Dir*>(include)) { dir->load(true, true); }
    }

    if (true) {
        auto file = File::load_path("/var/cache/apt/apt-file");
        if (auto dir = dynamic_cast<Dir*>(file)) { dir->load(true, true); }
    }

    if (argc == 1+1 and argv[1]) {
        auto file = File::load_path(argv[1]);
        if (auto dir = dynamic_cast<Dir*>(file)) {
            dir->load(true, true);
            cout << "Disk Usage (du): " << dir->get_tree_csize() << endl;
        }
        // } else {
        //     cout << "Usage: " << argv[0] << " <DIR>" << endl;
        //     return -1;
    }

    g_oreg->show(cout, 0, 1);
    semnet_main();

    return 0;
}

int semnet_main()
{
    int ret = 0;
    bool interactive_flag = true;
    bool quit_flag = false;
    for (int i = 0; not quit_flag; i++) {
        semnet::g_oreg->iter(1);
        //g_oreg->show(cout, 0, 10);

        if (interactive_flag) {
            char *line = readline_with_prompt("DVis> ");
            const char * quit_cmd = "quit";
            if (line) {
                if (cbuf_begins(line, strlen(line),
                                quit_cmd, strlen(quit_cmd))) { quit_flag = 1; }
                else {
                    char patt_cstr[128];
                    if (sscanf(line, "scan %s", patt_cstr) == 1) {
                        pTimer tmr; ptimer_init(&tmr, CLOCK_PROCESS_CPUTIME_ID); ptimer_tic(&tmr);
#if 0
                        Iter * iter = gen::iter(Dir::root(), REL_CHILD, OB_FILE_, PTRAV_BREADTH_FIRST_FORWARD,
                                                lit_r(patt_cstr));
                        Obs hits = iter->perform();
                        ptimer_toc(&tmr);
                        printf("Iteration took "); ptimer_print_sec_usec9(tmr);
                        printf(" and gave the %zd hits:\n", hits.size());
                        for (uint j = 0; j < hits.size(); j++) {
                            Ob * ob = hits[j];
                            if (ob) {
                                cout << "j:" << j << ": ";
                                ob->show(cout);
                                cout << endl;
                            }
                        }
#endif
                    }

                }
            }
            free(line);
        }
        if (quit_flag) { break; }
    }
    return ret;
}
