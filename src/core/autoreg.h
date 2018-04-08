/*
 * autoreg.h
 *
 *  Created on: Sep 17, 2012
 *      Author: jasonr
 */

#ifndef AUTOREG_H_
#define AUTOREG_H_

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

// \cond HIDDEN
// this class exists to fix a problem with gcc's linker removing references to objects not used in
// any of the linked files.  this comes up when using the factory pattern to create objects via a
// plugin system.  the dynamic loader can't find the stripped out object which the plugin is trying
// to create.
// inheriting a class from this autoregister struct creates (and uses) a static object of that class
// guaranteeing that it won't be stripped out by the linker.
//
// source : http://stackoverflow.com/questions/401621/best-way-to-build-a-list-of-per-type-data/401801#401801
template<typename D>
struct autoregister
{
    struct exec_register
    {
        exec_register()
        {
        }
    };

    // will force instantiation of definition of static member
    template<exec_register&>
    struct ref_it { };

    static exec_register register_object;
    static ref_it<register_object> referrer;
};

template<typename D>
typename autoregister<D>::exec_register autoregister<D>::register_object;
// \endcond

#endif // AUTOREG_H_
