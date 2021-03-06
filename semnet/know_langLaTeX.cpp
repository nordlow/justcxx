#include "know_langLaTeX.hpp"
#include "know_langC.hpp"
#include "lit.hpp"
#include "alt.hpp"
#include "lang.hpp"
#include "translate.hpp"

namespace semnet {
namespace patterns {

Lang* gp_LaTeX_lang = nullptr;

// \see http://www.artofproblemsolving.com/Wiki/index.php/LaTeX:Symbols
Lang* learn_LaTeX_lang()
{
    if (gp_LaTeX_lang) { return gp_LaTeX_lang; }

    using namespace semnet::patterns::gen;

    // Relational Operators
    auto rel    = alt(); rel->set_name("Relational Operator"); // TODO: translate(rel, gp_rel_ops);
    auto ge     = rel->add(lit("\\ge" "Greater Than or Equal To")->tokenize());
    auto le     = rel->add(lit("\\le", "Greater Than or Equal To")->tokenize());
    auto neq    = rel->add(lit("\\neq", "Not Equal To")->tokenize());
    auto sim    = rel->add(lit("\\sim", "Similar To")->tokenize());
    auto ll     = rel->add(lit("\\ll", "Much Less Than")->tokenize());
    auto gg     = rel->add(lit("\\gg", "Much Greater Than")->tokenize());
    auto doteq  = rel->add(lit("\\doteq", "Dot Equal To")->tokenize());
    auto simeq  = rel->add(lit("\\simeq", "Similar Equal To")->tokenize());

    auto subset = rel->add(lit("\\simeq", "Subset Of")->tokenize()); // operates on sets
    auto supset = rel->add(lit("\\simeq", "Superset Of")->tokenize()); // operates on set)s

    auto approx = rel->add(lit("\\simeq", "Approximately Equal To")->tokenize());
    auto isomorpic = rel->add(lit("\\cong", "Structurally Identical To (Isomorphic)")->tokenize());
    // TODO: homeomorphic
    // TODO: homotopic

    //
    auto asymb = rel->add(lit("\\asymp", "Asymptotically Equal To",
                              "If A(x) and B(x) are functions of x then A(x) asymptotic to B(x) means that the limit as x increases without bound of A(x) / B(x) exists and equals one.")->tokenize());

    // Set Operators
    auto subseteq = rel->add(lit("\\subseteq", "Subset Of Or Equal To")->tokenize());
    auto supseteq = rel->add(lit("\\supseteq", "Superset Of Or Equal To")->tokenize());
    auto sqsubset = rel->add(lit("\\sqsubset", "Square Subset Of")->tokenize());
    auto sqsupset = rel->add(lit("\\sqsupset", "Square Superset Of")->tokenize());
    auto sqsubseteq = rel->add(lit("\\sqsubseteq", "Square Subset Of Or Equal To")->tokenize());
    auto sqsupseteq = rel->add(lit("\\sqsupseteq", "Square Superset Of Or Equal To")->tokenize());

    auto smile = rel->add(lit("\\smile", "Smile")->tokenize());

    auto equiv = rel->add(lit("\\equiv", "Equivalent To")->tokenize());
    auto frown = rel->add(lit("\\frown", "Frown")->tokenize());

    auto propto = rel->add(lit("\\propto", "Proportional To")->tokenize());
    auto bowtie = rel->add(lit("\\bowtie", "Bowti")->tokenize());
    auto in = rel->add(lit("\\in", "In")->tokenize()); // on sets
    auto ni = rel->add(lit("\\ni", "Reversed In")->tokenize()); // on sets

    auto prec = rel->add(lit("\\prec", "Preceedes")->tokenize()); // on \c LessThanComparable
    auto succ = rel->add(lit("\\succ", "Succeeds")->tokenize()); // on \c LessThanComparable
    auto precq = rel->add(lit("\\precq", "Preceedes or Equal To")->tokenize()); // on \c LessThanComparable
    auto succq = rel->add(lit("\\succq", "Succeeds or Equal To")->tokenize()); // on \c LessThanComparable

    auto vdash = rel->add(lit("\\vdash")->tokenize());
    auto dashv = rel->add(lit("\\dash")->tokenize());

    auto models = rel->add(lit("\\models", "Models")->tokenize());

    auto mid = rel->add(lit("\\mid", "Conditional")->tokenize());

    // Geometric Relational Operators
    auto perp = rel->add(lit("\\perp", "Perpendicular (Orthogonal) To")->tokenize());
    auto parallel = rel->add(alt(lit("\\parallel", "Parallel To")->tokenize(),
                                 lit("\\|", "Parallel To")->tokenize()));

    auto greekLC = alt(); greekLC->set_name("Lowercase Greek Letter");
    auto alpha = greekLC->add(lit("\\alpha", "alpha")->tokenize());
    auto beta = greekLC->add(lit("\\beta", "beta")->tokenize());
    auto gamma = greekLC->add(lit("\\gamma", "gamma")->tokenize());
    auto delta = greekLC->add(lit("\\delta", "delta")->tokenize());
    auto epsilon = greekLC->add(lit("\\epsilon", "epsilon")->tokenize());
    auto varepsilon = greekLC->add(lit("\\varepsilon", "varepsilon")->tokenize());
    auto zeta = greekLC->add(lit("\\zeta", "zeta")->tokenize());
    auto eta = greekLC->add(lit("\\eta", "eta")->tokenize());
    auto theta = greekLC->add(lit("\\theta", "theta")->tokenize());
    auto vartheta = greekLC->add(lit("\\vartheta", "vartheta")->tokenize());
    auto iota = greekLC->add(lit("\\iota", "iota")->tokenize());
    auto kappa = greekLC->add(lit("\\kappa", "kappa")->tokenize());
    auto lambda = greekLC->add(lit("\\lambda", "lambda")->tokenize());
    auto mu = greekLC->add(lit("\\mu", "mu")->tokenize());
    auto nu = greekLC->add(lit("\\nu", "nu")->tokenize());
    auto xi = greekLC->add(lit("\\xi", "xi")->tokenize());
    auto pi = greekLC->add(lit("\\pi", "pi")->tokenize());
    auto varpi = greekLC->add(lit("\\varpi", "varpi")->tokenize());
    auto rho = greekLC->add(lit("\\rho", "rho")->tokenize());
    auto varrho = greekLC->add(lit("\\varrho", "varrho")->tokenize());
    auto sigma = greekLC->add(lit("\\sigma", "sigma")->tokenize());
    auto varsigma = greekLC->add(lit("\\varsigma", "varsigma")->tokenize());
    auto tau = greekLC->add(lit("\\tau", "tau")->tokenize());
    auto upsilon = greekLC->add(lit("\\upsilon", "upsilon")->tokenize());
    auto phi = greekLC->add(lit("\\phi", "phi")->tokenize());
    auto varphi = greekLC->add(lit("\\varphi", "varphi")->tokenize());
    auto chi = greekLC->add(lit("\\chi", "chi")->tokenize());
    auto psi = greekLC->add(lit("\\psi", "psi")->tokenize());
    auto omega = greekLC->add(lit("\\omega", "omega")->tokenize());

    auto greekUC = alt(); greekUC->set_name("Upppercase Greek Letter");
    auto Gamma = greekUC->add(lit("\\Gamma", "Gamma")->tokenize());
    auto Delta = greekUC->add(lit("\\Delta", "Delta")->tokenize());
    auto Theta = greekUC->add(lit("\\Theta", "Theta")->tokenize());
    auto Lambda = greekUC->add(lit("\\Lambda", "Lambda")->tokenize());
    auto Xi = greekUC->add(lit("\\Xi", "Xi")->tokenize());
    auto Pi = greekUC->add(lit("\\Pi", "Pi")->tokenize());
    auto Sigma = greekUC->add(lit("\\Sigma", "Sigma")->tokenize());
    auto Upsilon = greekUC->add(lit("\\Upsilon", "Upsilon")->tokenize());
    auto Phi = greekUC->add(lit("\\Phi", "Phi")->tokenize());
    auto Psi = greekUC->add(lit("\\Psi", "Psi")->tokenize());
    auto Omega = greekUC->add(lit("\\Omega", "Omega")->tokenize());

    return gp_LaTeX_lang = lang("LaTeX",
                                alt(rel, greekLC, greekUC, rel, ge, le, neq, sim,
                                    ll, gg, doteq, simeq, subset, supset, approx,
                                    isomorpic, asymb,
                                    subseteq, supseteq, sqsubset, sqsupset, sqsubseteq, sqsupseteq,
                                    smile, equiv, frown, propto, bowtie, in, ni, prec, succ, precq, succq, vdash, dashv,
                                    models, mid, perp, parallel, greekLC,
                                    alpha, beta, gamma, delta, epsilon, varepsilon, zeta, eta, theta, vartheta, iota, kappa, lambda,
                                    mu, nu, xi, pi, varpi, rho, varrho, sigma, varsigma, tau, upsilon, phi, varphi, chi, psi, omega,
                                    greekUC, Gamma, Delta, Theta, Lambda, Xi, Pi, Sigma, Upsilon, Phi, Psi, Omega
                                    ));
}

}
}
