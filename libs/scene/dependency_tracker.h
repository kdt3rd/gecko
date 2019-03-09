// Copyright (c) 2019 Kimball Thurston
// SPDX-License-Identifier: MIT

#pragma once

////////////////////////////////////////

namespace scene
{

///
/// @brief Class dependency_tracker provides a lightweight mechanism to track dependencies between otherwise un-connected objects.
///
/// This is most commonly used when there are expressions or similar that cross-link to nodes values
///
class dependency_tracker
{
public:
    class dependent
    {
    public:
        dependent( dependency_tracker &t );
        virtual ~dependent( void );

        void add_dependency( dependent *dep );

    protected:
        virtual void dependency_changed( dependent *obj ) = 0;
        virtual void dependency_removed( dependent *obj ) = 0;
        friend class dependency_tracker;
    };

    void notify_dependents_changed( dependent *dep );
    
protected:
    // will be auto-registered / removed in the
    // constructor / destructor of dependent
    void register_object( dependent *dep );
    void remove_object( dependent *dep );

    friend class dependent;

};

} // namespace scene



