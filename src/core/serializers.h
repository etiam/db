/*
 * serializers.h
 *
 *  Created on: Jan 7, 2015
 *      Author: jasonr
 */

#pragma once
#ifndef CORESERIALIZERS_H_
#define CORESERIALIZERS_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

#include "archive.h"

namespace Ast
{

struct ReferenceLocation;
struct ReferenceData;

}

namespace Core
{

void serialize(Archive &ar, bool &t);
void serialize(Archive &ar, int &t);
void serialize(Archive &ar, unsigned int &t);
void serialize(Archive &ar, unsigned short &t);

void serialize(Archive &ar, unsigned long &t);
void serialize(Archive &ar, const unsigned long t);

void serialize(Archive &ar, float &t);
void serialize(Archive &ar, std::string &t);

void serialize(Archive &ar, Ast::ReferenceLocation &t);
void serialize(Archive &ar, Ast::ReferenceData &t);

// std::unique_ptr
template<typename T>
void
serialize(Archive &ar, std::unique_ptr<T> &t)
{
    if (ar.isRead())
        t.reset(new T());
    ar & t.get();
}

template <typename T>
void
serialize(Archive &ar, std::vector<T> &t)
{
    if (ar.isRead())
    {
        decltype(t.size()) size;

        ar.read((char*)&size, sizeof(size));

        t.resize(size);
        ar.read((char*)&t[0], size * sizeof(T));
    }
    else
    {
        auto size = t.size();
        ar.write((char*)&size, sizeof(size));
        ar.write((char*)&t[0], size * sizeof(T));
    }
}

template <typename K, typename V>
void
serialize(Archive &ar, std::unordered_map<K, V> &t)
{
    if (ar.isRead())
    {
//        decltype(t.size()) size;
//
//        ar.read((char*)&size, sizeof(size));
//
//        t.resize(size);
//        ar.read((char*)&t[0], size * sizeof(K));
    }
    else
    {
        // write size
        auto size = t.size();
        ar.write((char*)&size, sizeof(size));

        // write data
        for (auto &item : t)
        {
            auto key = item.first;
            ar << key;
            auto val = item.second;
            ar << val;
        }
    }
}


}

#endif
