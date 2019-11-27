/* ***** BEGIN LICENSE BLOCK *****
 * Version: MPL 1.1/GPL 2.0/LGPL 2.1
 *
 * The contents of this file are subject to the Mozilla Public License Version
 * 1.1 (the "License"); you may not use this file except in compliance with
 * the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
 * for the specific language governing rights and limitations under the
 * License.
 *
 * The Original Code is Mozilla Universal charset detector code.
 *
 * The Initial Developer of the Original Code is
 * Netscape Communications Corporation.
 * Portions created by the Initial Developer are Copyright (C) 2001
 * the Initial Developer. All Rights Reserved.
 *
 * Contributor(s):
 *          Jehan <jehan@girinstud.io>
 *
 * Alternatively, the contents of this file may be used under the terms of
 * either the GNU General Public License Version 2 or later (the "GPL"), or
 * the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
 * in which case the provisions of the GPL or the LGPL are applicable instead
 * of those above. If you wish to allow use of your version of this file only
 * under the terms of either the GPL or the LGPL, and not to allow others to
 * use your version of this file under the terms of the MPL, indicate your
 * decision by deleting the provisions above and replace them with the notice
 * and other provisions required by the GPL or the LGPL. If you do not delete
 * the provisions above, a recipient may use your version of this file under
 * the terms of any one of the MPL, the GPL or the LGPL.
 *
 * ***** END LICENSE BLOCK ***** */

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../src/uchardet.h"

#define BUFFER_SIZE 65536

char *
detect(FILE *fp)
{
    uchardet_t  handle = uchardet_new();
    char       *charset;
    char        buffer[BUFFER_SIZE];
    int         i;

    while (!feof(fp))
    {
        size_t len = fread(buffer, 1, BUFFER_SIZE, fp);
        int retval = uchardet_handle_data(handle, buffer, len);
        if (retval != 0)
        {
            fprintf(stderr,
                    "uchardet-tests: handle data error.\n");
            exit(1);
        }
    }
    uchardet_data_end(handle);

    charset = strdup(uchardet_get_charset(handle));
    for (i = 0; charset[i]; i++)
    {
        /* Our test files are lowercase. */
        charset[i] = tolower(charset[i]);
    }

    uchardet_delete(handle);

    return charset;
}

int
main(int argc, char ** argv)
{
    FILE *f;
    char *filename;
    char *expected_charset;
    char *charset;
    int   success;

    if (argc != 2)
    {
        /* The test program expects exactly 1 argument. */
        fprintf(stderr,
                "uchardet-tests expects exactly 1 argument\n");
        return 1;
    }

    filename = strdup(argv[1]);
    f = fopen(filename, "r");
    if (f == NULL)
    {
        /* Error opening the test file. */
        fprintf(stderr,
                "uchardet-tests: error opening the test file\n");
        return 1;
    }

    expected_charset = strrchr(filename, '/');
    if (expected_charset == NULL)
    {
        expected_charset = filename;
    }
    else
    {
        expected_charset++;
    }
    expected_charset = strtok(expected_charset, ".");

    charset = detect(f);
    fclose (f);

    /* In a unit test, 0 means success, other returned values mean failure. */
    success = (strcmp(charset, expected_charset) != 0);

    free(charset);
    free(filename);

    return success;
}
