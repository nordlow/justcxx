/*!
 * \file Canberra.hpp
 * \brief C++ Wrapper for libcanberra implementing the XDG Sound Naming (Theme).
 * \author Copyright (C) 2012 Per Nordlöw (per.nordlow@gmail.com)
 * \date 2009-06-11 08:52
 *
 * \see http://0pointer.de/public/sound-naming-spec.html
 */

#pragma once

#include <cstdlib>
#include <canberra.h>

/*!
 * C++ Wrapper Class for \c libcanberra API
 */
class Canberra
{
public:
    /*!
     * Default Constructor.
     * \param app_name for example "An example"
     * \param app_id for example "org.freedesktop.libcanberra.Test"
     * \param app_icon_name for example "libcanberra-test"
     */
    Canberra(const char* app_name  = NULL,
             const char* app_id    = NULL,
             const char* app_icon_name = NULL) {
        ca_context_create(&m_ctx);
        /* Set a few application-global properties */
        if (app_name) { ca_context_change_props(m_ctx, CA_PROP_APPLICATION_NAME, app_name, NULL); }
        if (app_id) { ca_context_change_props(m_ctx, CA_PROP_APPLICATION_ID, app_id, NULL); }
        if (app_icon_name) { ca_context_change_props(m_ctx, CA_PROP_APPLICATION_ICON_NAME, app_icon_name, NULL); }
    }

    /// Destructor.
    ~Canberra() { ca_context_destroy(m_ctx) ;}

    /// Trigger a sound named \p name, for example "button-pressed";
    void play(const char* name) {
        ca_context_play(m_ctx, 0,
                        CA_PROP_EVENT_ID, name, /* The XDG sound name */
                        // CA_PROP_MEDIA_NAME, "The user pressed the button foobar",
                        // CA_PROP_EVENT_MOUSE_X, "555",
                        // CA_PROP_EVENT_MOUSE_Y, "666",
                        // CA_PROP_WINDOW_NAME, "Foobar Dialog",
                        // CA_PROP_WINDOW_ICON_NAME, "libcanberra-test-foobar-dialog",
                        // CA_PROP_WINDOW_X11_DISPLAY, ":0",
                        // CA_PROP_WINDOW_X11_XID, "4711",
                        NULL);
    }

protected:
    /// Copy Constructor.
    Canberra(const Canberra& in) {}
private:
    ca_context *m_ctx;
};
