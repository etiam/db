/*
 * ast.cpp
 *
 *  Created on: Mar 23, 2018
 *      Author: jasonr
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <cstring>
#include <iostream>
#include <unordered_map>
#include <tuple>

#include <clang-c/Index.h>
#include <clang-c/CXCompilationDatabase.h>

#include <ast/astBuilder.h>

namespace
{

struct ReferenceLocation
{
    unsigned int line, colstart, colend;

    std::size_t
    hash() const
    {
        // Compute individual hash values for first, second and third (http://stackoverflow.com/a/1646913/126995)
        std::size_t hash = 17;
        hash = hash * 31 + std::hash<int>()(line);
        hash = hash * 31 + std::hash<int>()(colstart);
        hash = hash * 31 + std::hash<int>()(colend);
        return hash;
    }

    bool
    operator==(const ReferenceLocation &other) const
    {
        return (line == other.line && colstart == other.colstart && colend == other.colend);
    }
};

}

namespace std
{

template <>
struct hash<::ReferenceLocation>
{
  public:
    std::size_t
    operator()(const ::ReferenceLocation &key) const
    {
        return key.hash();
    }
};

}

namespace Ast
{

class AstBuilderImpl
{
  public:
    AstBuilderImpl();
    ~AstBuilderImpl();

    void                        setBuildPath(const std::string &path);
    void                        parseFile(const std::string &filename);
    void                        addReference(CXCursor cursor, CXCursor parent);

    static CXChildVisitResult   visitFunction(CXCursor cursor, CXCursor parent, CXClientData client_data);

    struct ReferenceData
    {
        unsigned int offset;
        std::string filename;
    };

    using References = std::unordered_map<::ReferenceLocation, ReferenceData>;

    std::string             m_buildPath;
    References              m_references;
    CXIndex                 m_index = nullptr;
    CXCompilationDatabase   m_compdb = nullptr;
};

AstBuilderImpl::AstBuilderImpl()
{
}

AstBuilderImpl::~AstBuilderImpl()
{
    clang_disposeIndex(m_index);

    if (m_compdb)
        clang_CompilationDatabase_dispose(m_compdb);
}

void
AstBuilderImpl::setBuildPath(const std::string &path)
{
    CXCompilationDatabase_Error errorcode;
    m_compdb = clang_CompilationDatabase_fromDirectory(path.c_str(), &errorcode);

    m_index = clang_createIndex(0, 0);
}

void
AstBuilderImpl::parseFile(const std::string &filename)
{
    auto unit = clang_parseTranslationUnit(m_index, filename.c_str(), nullptr, 0, nullptr, 0, CXTranslationUnit_None);

    if (unit == nullptr)
    {
        std::cerr << "AstBuilder::parseFile(): unable to parse \"" << filename << "\"." << std::endl;
    }
    else
    {
        CXCursor cursor = clang_getTranslationUnitCursor(unit);
        clang_visitChildren(cursor, visitFunction, this);
    }

    clang_disposeTranslationUnit(unit);
}

CXChildVisitResult
AstBuilderImpl::visitFunction(CXCursor cursor, CXCursor parent, CXClientData client_data)
{
    auto *parser = static_cast<AstBuilderImpl *>(client_data);

    switch(clang_getCursorKind(cursor))
    {
        case CXCursor_VarDecl :
        case CXCursor_FieldDecl :
            parser->addReference(cursor, parent);
            break;

        default : break;
    }
    return CXChildVisit_Recurse;
}

void
AstBuilderImpl::addReference(CXCursor cursor, CXCursor parent)
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
            m_references[::ReferenceLocation({line, column, column + varlen - 1})] = ReferenceData({offset, clang_getCString(filename)});
            clang_disposeString(filename);

//            std::cout << "'" << cursorspelling << "' "
//                      << "of kind '" << clang_getCursorKindSpelling(clang_getCursorKind(cursor)) << "' "
//                      << "(" << clang_getCursorKind(cursor) << ") " << clang_getFileName(file) << ":" << line << ":" << column << "-"
//                      << column + varlen - 1 << std::endl;
        }
    }
}

AstBuilder::AstBuilder() :
    m_impl(std::make_unique<AstBuilderImpl>())
{
}

AstBuilder::~AstBuilder()
{
}

void
AstBuilder::setBuildPath(const std::string &path)
{
    m_impl->setBuildPath(path);
}

void
AstBuilder::parseFile(const std::string &filename)
{
    m_impl->parseFile(filename);

    for (const auto ref : m_impl->m_references)
    {
        auto key = ref.first;
        auto val = ref.second;

        std::cout << val.filename << ":" << key.line << " " << key.colstart << "-" << key.colend << " (" << val.offset << ")" << std::endl;
    }
}

}
