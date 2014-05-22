/*! \file rewritersample.cpp
 * \brief Source-to-source transformation sample with Clang, using Rewriter - the code rewriting interface.
 * \author Eli Bendersky (eliben@gmail.com)
 * \see http://eli.thegreenplace.net/2012/06/08/basic-source-to-source-transformation-with-clang/
 */

#include <cstdio>
#include <string>
#include <sstream>

#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Basic/Diagnostic.h"
#include "clang/Basic/FileManager.h"
#include "clang/Basic/SourceManager.h"
#include "clang/Basic/TargetOptions.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Lex/Preprocessor.h"
#include "clang/Parse/ParseAST.h"
#include "clang/Rewrite/Rewriter.h"
#include "clang/Rewrite/Rewriters.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_ostream.h"

using namespace clang;
using namespace std;

// By implementing RecursiveASTVisitor, we can specify which AST nodes
// we're interested in by overriding relevant methods.
class MyASTVisitor : public RecursiveASTVisitor<MyASTVisitor>
{
public:
    MyASTVisitor(Rewriter &R) : m_rw(R) {}

    /*! Visit Statement \p s. */
    bool VisitStmt(Stmt *s) {
        // Only care about If statements.
        if (isa<IfStmt>(s)) {
            IfStmt *IfStatement = cast<IfStmt>(s);
            Stmt *Then = IfStatement->getThen();
            m_rw.InsertText(Then->getLocStart(),
                                   "// the 'if' part\n",
                                   true, true);
            Stmt *Else = IfStatement->getElse();
            if (Else)
                m_rw.InsertText(Else->getLocStart(),
                                       "// the 'else' part\n",
                                       true, true);
        }
        return true;
    }

    /*! Visit Function Declaration \p f. */
    bool VisitFunctionDecl(FunctionDecl *f) {
        // Only function definitions (with bodies), not declarations.
        if (f->hasBody()) {
            Stmt *FuncBody = f->getBody();

            // Type name as string
            QualType QT = f->getResultType();
            string TypeStr = QT.getAsString();

            // Function name
            DeclarationName DeclName = f->getNameInfo().getName();
            string FuncName = DeclName.getAsString();

            // Add comment before
            stringstream SSBefore;
            SSBefore << "// Begin function " << FuncName << " returning "
                     << TypeStr << "\n";
            SourceLocation ST = f->getSourceRange().getBegin();
            m_rw.InsertText(ST, SSBefore.str(), true, true);

            // And after
            stringstream SSAfter;
            SSAfter << "\n// End function " << FuncName << "\n";
            ST = FuncBody->getLocEnd().getLocWithOffset(1);
            m_rw.InsertText(ST, SSAfter.str(), true, true);
        }

        return true;
    }

private:
    void AddBraces(Stmt *s);
    Rewriter &m_rw;
};

// Implementation of the ASTConsumer interface for reading an AST produced by the Clang parser.
class MyASTConsumer : public ASTConsumer
{
public:
    MyASTConsumer(Rewriter &R) : Visitor(R) {}
    // Override the method that gets called for each parsed top-level
    // declaration.
    virtual bool HandleTopLevelDecl(DeclGroupRef DR) {
        for (DeclGroupRef::iterator b = DR.begin(), e = DR.end();
             b != e; ++b)
            // Traverse the declaration using our AST visitor.
            Visitor.TraverseDecl(*b);
        return true;
    }
private:
    MyASTVisitor Visitor;
};

int main(int argc, char *argv[])
{
    if (argc != 2) {
        llvm::errs() << "Usage: rewritersample <filename>\n";
        return 1;
    }

    // CompilerInstance will hold the instance of the Clang compiler for us,
    // managing the various objects needed to run the compiler.
    CompilerInstance ci;
    ci.createDiagnostics(0, 0);

    // Initialize target info with the default triple for our platform.
    TargetOptions TO;
    TO.Triple = llvm::sys::getDefaultTargetTriple();
    TargetInfo *TI = TargetInfo::CreateTargetInfo(
        ci.getDiagnostics(), TO);
    ci.setTarget(TI);

    ci.createFileManager();
    FileManager &fm = ci.getFileManager();
    ci.createSourceManager(fm);
    SourceManager &SourceMgr = ci.getSourceManager();
    ci.createPreprocessor();
    ci.createASTContext();

    // A Rewriter helps us manage the code rewriting task.
    Rewriter rw;
    rw.setSourceMgr(SourceMgr, ci.getLangOpts());

    // Set the main file handled by the source manager to the input file.
    const FileEntry *fi = fm.getFile(argv[1]);
    SourceMgr.createMainFileID(fi);
    ci.getDiagnosticClient().BeginSourceFile(
        ci.getLangOpts(),
        &ci.getPreprocessor());

    // Create an AST consumer instance which is going to get called by
    // ParseAST.
    MyASTConsumer astc(rw);

    // Parse the file to AST, registering our consumer as the AST consumer.
    ParseAST(ci.getPreprocessor(), &astc,
             ci.getASTContext());

    // At this point the rewriter's buffer should be full with the rewritten
    // file contents.
    const RewriteBuffer *rwb =
        rw.getRewriteBufferFor(SourceMgr.getMainFileID());
    llvm::outs() << string(rwb->begin(), rwb->end());

    return 0;
}
