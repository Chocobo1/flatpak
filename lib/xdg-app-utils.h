/*
 * Copyright © 2014 Red Hat, Inc
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *       Alexander Larsson <alexl@redhat.com>
 */

#ifndef __XDG_APP_UTILS_H__
#define __XDG_APP_UTILS_H__

#include <string.h>

#include "libglnx/libglnx.h"
#include <gio/gio.h>
#include <libsoup/soup.h>
#include "xdg-app-dbus.h"
#include "xdg-app-dir.h"
#include <ostree.h>

const char * xdg_app_get_arch (void);

gboolean xdg_app_has_name_prefix (const char *string,
                                  const char *name);
gboolean xdg_app_is_valid_name (const char *string);
gboolean xdg_app_is_valid_branch (const char *string);

char * xdg_app_build_untyped_ref (const char *runtime,
                                  const char *branch,
                                  const char *arch);
char * xdg_app_build_runtime_ref (const char *runtime,
                                  const char *branch,
                                  const char *arch);
char * xdg_app_build_app_ref (const char *app,
                              const char *branch,
                              const char *arch);
GFile * xdg_app_find_deploy_dir_for_ref (const char *ref,
                                         GCancellable *cancellable,
                                         GError **error);
XdgAppDeploy * xdg_app_find_deploy_for_ref (const char *ref,
                                            GCancellable *cancellable,
                                            GError **error);
char ** xdg_app_list_deployed_refs (const char *type,
				    const char *name_prefix,
				    const char *branch,
				    const char *arch,
				    GCancellable *cancellable,
				    GError **error);

gboolean xdg_app_overlay_symlink_tree (GFile    *source,
                                       GFile    *destination,
                                       const char *symlink_prefix,
                                       GCancellable  *cancellable,
                                       GError       **error);
gboolean xdg_app_remove_dangling_symlinks (GFile    *dir,
                                           GCancellable  *cancellable,
                                           GError       **error);

gboolean ostree_repo_load_summary (const char *repository_url,
                                   GHashTable **refs,
                                   gchar **title,
                                   GCancellable *cancellable,
                                   GError **error);

#if !GLIB_CHECK_VERSION(2,43,1)
static inline  gboolean
g_strv_contains (const gchar * const *strv,
                 const gchar         *str)
{
  g_return_val_if_fail (strv != NULL, FALSE);
  g_return_val_if_fail (str != NULL, FALSE);

  for (; *strv != NULL; strv++)
    {
      if (g_str_equal (str, *strv))
        return TRUE;
    }

  return FALSE;
}
#endif

#if !GLIB_CHECK_VERSION(2,40,0)
static inline gboolean
g_key_file_save_to_file (GKeyFile     *key_file,
			 const gchar  *filename,
			 GError      **error)
{
  gchar *contents;
  gboolean success;
  gsize length;

  contents = g_key_file_to_data (key_file, &length, NULL);
  success = g_file_set_contents (filename, contents, length, error);
  g_free (contents);

  return success;
}
#endif

/* Returns the first string in subset that is not in strv */
static inline const gchar *
g_strv_subset (const gchar * const *strv,
               const gchar * const *subset)
{
  int i;

  for (i = 0; subset[i]; i++)
    {
      const char *key;

      key = subset[i];
      if (!g_strv_contains (strv, key))
        return key;
    }

  return NULL;
}

static inline void xdg_app_auto_unlock_helper (GMutex **mutex)
{
  if (*mutex)
    g_mutex_unlock (*mutex);
}

static inline GMutex *xdg_app_auto_lock_helper (GMutex *mutex)
{
  if (mutex)
    g_mutex_lock (mutex);
  return mutex;
}


#define AUTOLOCK(name) __attribute__((cleanup(xdg_app_auto_unlock_helper))) GMutex * G_PASTE(auto_unlock, __LINE__) = xdg_app_auto_lock_helper (&G_LOCK_NAME (name))

G_DEFINE_AUTOPTR_CLEANUP_FUNC(OstreeRepo, g_object_unref)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(OstreeMutableTree, g_object_unref)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(OstreeAsyncProgress, g_object_unref)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(SoupSession, g_object_unref)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(SoupMessage, g_object_unref)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(XdgAppSessionHelper, g_object_unref)

#if !GLIB_CHECK_VERSION(2, 43, 4)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(GFile, g_object_unref)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(GFileEnumerator, g_object_unref)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(GFileInfo, g_object_unref)
G_DEFINE_AUTOPTR_CLEANUP_FUNC(GOutputStream, g_object_unref)
#endif

#endif /* __XDG_APP_UTILS_H__ */