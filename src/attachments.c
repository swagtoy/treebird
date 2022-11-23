/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#include <string.h>
#include <stdlib.h>
#include "base_page.h"
#include "helpers.h"
#include "easprintf.h"
#include "attachments.h"
#include "string_helpers.h"

struct attachments_args
{
    struct session* ssn;
    struct mstdnt_attachment* atts;
    mstdnt_bool sensitive;
};

int try_upload_media(struct mstdnt_storage** storage,
                     struct session* ssn,
                     mastodont_t* api,
                     struct mstdnt_attachment** attachments,
                     char*** media_ids)
{
    #if 0
    struct mstdnt_args m_args;
    set_mstdnt_args(&m_args, ssn);
    size_t size = keyfile(ssn->post.files).array_size;
    if (!FILES_READY(ssn))
        return 1;

    if (media_ids)
        *media_ids = tb_malloc(sizeof(char*) * size);

    *attachments = tb_malloc(sizeof(struct mstdnt_attachment) * size);
    *storage = tb_calloc(1, sizeof(struct mstdnt_storage) * size);

    for (int i = 0; i < size; ++i)
    {
        struct file_content* content = keyfile(ssn->post.files).content + i;
        struct mstdnt_upload_media_args args = {
            .file = {
                .file = content->content,
                .filename = content->filename,
                .filesize = content->content_size,
                .filetype = content->filetype,
            },
            .thumbnail = NULL,
            .description = NULL,
        };
        
        if (mstdnt_upload_media(api,
                                &m_args,
                                request_cb_try_upload_media, upload_media_cb_args,
                                args))
        {
            for (size_t j = 0; j < i; ++j)
            {
                if (media_ids) tb_free((*media_ids)[j]);
                mstdnt_storage_cleanup(*storage + j);
            }

            if (media_ids)
            {
                tb_free(*media_ids);
                *media_ids = NULL;
            }
            
            tb_free(*attachments);
            *attachments = NULL;
            tb_free(*storage);
            *storage = NULL;
            return 1;
        }

        if (media_ids)
        {
            (*media_ids)[i] = tb_malloc(strlen((*attachments)[i].id)+1);
            strcpy((*media_ids)[i], (*attachments)[i].id);
        }
    }
    
    return 0;
#endif
}

void cleanup_media_storages(struct session* ssn, struct mstdnt_storage* storage)
{
#if 0
    if (!FILES_READY(ssn)) return;
    for (size_t i = 0; i < keyfile(ssn->post.files).array_size; ++i)
        mstdnt_storage_cleanup(storage + i);
    tb_free(storage);
#endif
}

void cleanup_media_ids(struct session* ssn, char** media_ids)
{
#if 0
    if (!FILES_READY(ssn)) return;
    if (!media_ids) return;
    for (size_t i = 0; i < keyfile(ssn->post.files).array_size; ++i)
        tb_free(media_ids[i]);
    tb_free(media_ids);
#endif
}

HV* perlify_attachment(const struct mstdnt_attachment* const attachment)
{
    if (!attachment) return NULL;
    HV* attach_hv = newHV();
    hvstores_str(attach_hv, "id", attachment->id);
    hvstores_int(attach_hv, "type", attachment->type);
    hvstores_str(attach_hv, "url", attachment->url);
    hvstores_str(attach_hv, "preview_url", attachment->preview_url);
    hvstores_str(attach_hv, "remote_url", attachment->remote_url);
    hvstores_str(attach_hv, "hash", attachment->hash);
    hvstores_str(attach_hv, "description", attachment->description);
    hvstores_str(attach_hv, "blurhash", attachment->blurhash);
    return attach_hv;
}

PERLIFY_MULTI(attachment, attachments, mstdnt_attachment)

int api_attachment_create(PATH_ARGS)
{
    struct mstdnt_storage *att_storage = NULL;
    struct mstdnt_attachment* attachments = NULL;
    char* string;
    char** media_ids = NULL;

    cJSON* root = cJSON_CreateObject();

    // TODO If multiple attachments are submitted, this uploads all of them.
    //      I don't think we want that, but it's just a minor issue and won't happen on TreebirdFE.
    try_upload_media(&att_storage, ssn, api, &attachments, &media_ids);

    if (media_ids)
        cJSON_AddStringToObject(root, "id", media_ids[0]);
    
    if (media_ids)
    {
        string = cJSON_Print(root);
        send_result(req, NULL, "application/json", string, 0);
        tb_free(string);
    }
    else
        send_result(req, NULL, "application/json", "{\"status\":\"Couldn't\"}", 0);

    // Cleanup media stuff
    cleanup_media_storages(ssn, att_storage);
    cleanup_media_ids(ssn, media_ids);
    tb_free(attachments);
    cJSON_Delete(root);
}
