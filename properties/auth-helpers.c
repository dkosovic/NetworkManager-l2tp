/* -*- Mode: C; tab-width: 4; indent-tabs-mode: t; c-basic-offset: 4 -*- */
/***************************************************************************
 *
 * Copyright (C) 2008 - 2010 Dan Williams, <dcbw@redhat.com>
 * Copyright (C) 2008 - 2015 Red Hat, Inc.
 * Copyright (C) 2019 Douglas Kosovic, <doug@uq.edu.au>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 **************************************************************************/

#include "nm-default.h"

#include "auth-helpers.h"

#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "nm-utils/nm-shared-utils.h"
#include "shared/utils.h"
#include "shared/nm-l2tp-crypto-openssl.h"

void
show_password (GtkToggleButton *togglebutton, GtkEntry *password_entry)
{
	gtk_entry_set_visibility (password_entry, gtk_toggle_button_get_active (togglebutton));
}

static gboolean
file_has_extension (const char *filename, const char *const*extensions)
{
	const char *p;
	gs_free char *ext = NULL;
	struct stat statbuf;

	if (!filename)
		return FALSE;

	p = strrchr (filename, '.');
	if (!p)
		return FALSE;

	/* Ignore files that are really large */
	if (!stat (filename, &statbuf)) {
		if (statbuf.st_size > 500000)
			return FALSE;
	}

	ext = g_ascii_strdown (p, -1);
	return g_strv_contains (extensions, ext);
}

static gboolean
cert_filter (const GtkFileFilterInfo *filter_info, gpointer data)
{
	static const char *const extensions[] = { ".der", ".pem", ".crt", ".cer", ".p12", NULL };

	return file_has_extension (filter_info->filename, extensions);
}


static gboolean
privkey_filter (const GtkFileFilterInfo *filter_info, gpointer user_data)
{
	static const char *const extensions[] = { ".der", ".pem", ".p12", ".key", NULL };

	return file_has_extension (filter_info->filename, extensions);
}

GtkFileFilter *
tls_cert_filter (void)
{
	GtkFileFilter *filter;

	filter = gtk_file_filter_new ();
	gtk_file_filter_add_custom (filter, GTK_FILE_FILTER_FILENAME, cert_filter, NULL, NULL);
	gtk_file_filter_set_name (filter, _("DER, PEM, or PKCS#12 certificates (*der, *.pem, *.crt, *.cer, *.p12)"));

	return filter;
}


GtkFileFilter *
tls_key_filter (void)
{
	GtkFileFilter *filter;

	filter = gtk_file_filter_new ();
	gtk_file_filter_add_custom (filter, GTK_FILE_FILTER_FILENAME, privkey_filter, NULL, NULL);
	gtk_file_filter_set_name (filter, _("DER, PEM, or PKCS#8 private keys (*.der, *.pem, *.pk8, *.key)"));

	return filter;
}

GtkFileFilter *
all_files_filter (void)
{
	GtkFileFilter *filter;

	filter = gtk_file_filter_new ();
	gtk_file_filter_add_pattern(filter, "*");
	gtk_file_filter_set_name (filter, _("All Files"));

	return filter;
}

