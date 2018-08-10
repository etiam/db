/*
 * scanner.cpp
 *
 *  Created on: Mar 23, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstring>
#include <iostream>
#include <iostream>
#include <tuple>
#include <boost/filesystem.hpp>

#include <clang-c/Index.h>
#include <clang-c/CXCompilationDatabase.h>

#include "ast/scanner.h"


namespace Ast
{

class ScannerImpl
{
  public:
    ScannerImpl();
    ~ScannerImpl();

    void setBuildPath(const std::string &buildpath);
    void parseFunctions(const std::string &filename);
    void addReference(CXCursor cursor, CXCursor parent);

    static CXChildVisitResult parseFunctionsVisitor(CXCursor cursor, CXCursor parent, CXClientData client_data);

    std::string m_buildPath;
    References m_functions;
    CXIndex m_index = nullptr;
    CXCompilationDatabase m_compdb = nullptr;
};

ScannerImpl::ScannerImpl()
{
    m_index = clang_createIndex(0, 0);
}

ScannerImpl::~ScannerImpl()
{
    if (m_index)
        clang_disposeIndex(m_index);

    if (m_compdb)
        clang_CompilationDatabase_dispose(m_compdb);
}

void
ScannerImpl::setBuildPath(const std::string &buildpath)
{
    auto ccfilename = boost::filesystem::path(buildpath) / "compile_commands.json";
    if (boost::filesystem::exists(ccfilename))
    {
        CXCompilationDatabase_Error errorcode;
        m_compdb = clang_CompilationDatabase_fromDirectory(buildpath.c_str(), &errorcode);
    }
}

void
ScannerImpl::parseFunctions(const std::string &filename)
{
    if (m_index && boost::filesystem::exists(filename))
    {
        auto unit = clang_parseTranslationUnit(m_index, filename.c_str(), nullptr, 0, nullptr, 0, CXTranslationUnit_SkipFunctionBodies);

        if (unit == nullptr)
        {
            std::cerr << "Scanner::parseFile(): unable to parse \"" << filename << "\"." << std::endl;
        }
        else
        {
            CXCursor cursor = clang_getTranslationUnitCursor(unit);
            clang_visitChildren(cursor, parseFunctionsVisitor, this);
        }

        clang_disposeTranslationUnit(unit);
    }
}

CXChildVisitResult
ScannerImpl::parseFunctionsVisitor(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    auto *parser = static_cast<ScannerImpl *>(client_data);

    switch(clang_getCursorKind(cursor))
    {
//        case CXCursor_VarDecl :
//        case CXCursor_FieldDecl :
        case CXCursor_FunctionDecl :
            parser->addReference(cursor, parent);
            break;

        default : break;
    }
    return CXChildVisit_Recurse;
}

void
ScannerImpl::addReference(CXCursor cursor, CXCursor parent)
{
    CXString cursorspelling = clang_getCursorSpelling(cursor);
    auto varlen = static_cast<unsigned int>(strlen(clang_getCString(cursorspelling)));
    if (varlen > 0)
    {
        CXFile file;
        unsigned int line, column, offset;

        clang_getSpellingLocation(clang_getCursorLocation(cursor), &file, &line, &column, &offset);

        if (file)
        {
            auto filename = clang_getFileName(file);
            auto spelling = clang_getCursorSpelling(cursor);
            m_functions[ReferenceLocation({line, column, column + varlen - 1})] =
                ReferenceData({offset, clang_getCString(spelling), clang_getCString(filename)});
            clang_disposeString(spelling);
            clang_disposeString(filename);
        }
    }
}

Scanner::Scanner() :
    m_impl(std::make_shared<ScannerImpl>())
{
}

Scanner::~Scanner()
{
}

void
Scanner::setBuildPath(const std::string &path)
{
    m_buildPath = path;
    m_impl->setBuildPath(path);
}

void
Scanner::parseFunctions(const std::string &filename)
{
    m_impl->parseFunctions(filename);

}

const References &
Scanner::functions() const
{
    return m_impl->m_functions;
}

}
