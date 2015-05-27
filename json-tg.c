#include "config.h"
#ifdef USE_JSON

#include <jansson.h>
#include "json-tg.h"
#include <tgl/tgl.h>
#include <tgl/tgl-layout.h>
#include <assert.h>

#ifndef json_boolean
#define json_boolean(val)      ((val) ? json_true() : json_false())
#endif

extern struct tgl_state *TLS;

void json_pack_peer_type (json_t *res, tgl_peer_id_t id) {
  int x = tgl_get_peer_type (id);
  switch (x) {
  case TGL_PEER_USER:
    assert (json_object_set (res, "type", json_string ("user")) >= 0);
    break;
  case TGL_PEER_CHAT:
    assert (json_object_set (res, "type", json_string ("chat")) >= 0);
    break;
  case TGL_PEER_ENCR_CHAT:
    assert (json_object_set (res, "type", json_string ("encr_chat")) >= 0);
    break;
  default:
    assert (0);
  }
}


void json_pack_user (json_t *res, tgl_peer_t *P) {
  if (P->user.first_name) {
    assert (json_object_set (res, "first_name", json_string (P->user.first_name)) >= 0);
  }
  if (P->user.last_name) {
    assert (json_object_set (res, "last_name", json_string (P->user.last_name)) >= 0);
  }
  if (P->user.real_first_name) {
    assert (json_object_set (res, "real_first_name", json_string (P->user.real_first_name)) >= 0);
  }
  if (P->user.real_last_name) {
    assert (json_object_set (res, "real_last_name", json_string (P->user.real_last_name)) >= 0);
  }
  if (P->user.phone) {
    assert (json_object_set (res, "phone", json_string (P->user.phone)) >= 0);
  }
  if (P->user.username) {
    assert (json_object_set (res, "username", json_string (P->user.username)) >= 0);
  }
}

void json_pack_chat (json_t *res, tgl_peer_t *P) {
  assert (P->chat.title);
  assert (json_object_set (res, "title", json_string (P->chat.title)) >= 0);
  assert (json_object_set (res, "members_num", json_integer (P->chat.users_num)) >= 0);

  if (P->chat.user_list) {
    json_t *m = json_array ();
    assert (m);

    int i;
    for (i = 0; i < P->chat.users_num; i++) {
      tgl_peer_id_t id = TGL_MK_USER (P->chat.user_list[i].user_id);
      assert (json_array_append (m, json_pack_peer (id, tgl_peer_get (TLS, id))) >= 0);
    }

    assert (json_object_set (res, "members", m) >= 0);
  }
}


void json_pack_encr_chat (json_t *res, tgl_peer_t *P) {
  assert (json_object_set (res, "user", json_pack_peer (TGL_MK_USER (P->encr_chat.user_id), tgl_peer_get (TLS, TGL_MK_USER (P->encr_chat.user_id)))) >= 0);
}

json_t *json_pack_peer (tgl_peer_id_t id, tgl_peer_t *P) {
  json_t *res = json_object ();
  assert (json_object_set (res, "id", json_integer (tgl_get_peer_id (id))) >= 0);

  json_pack_peer_type (res, id);

  assert (res);
    
  if (!P || !(P->flags & TGLPF_CREATED)) {
    static char s[100];
    switch (tgl_get_peer_type (id)) {
    case TGL_PEER_USER:
      sprintf (s, "user#%d", tgl_get_peer_id (id));
      break;
    case TGL_PEER_CHAT:
      sprintf (s, "chat#%d", tgl_get_peer_id (id));
      break;
    case TGL_PEER_ENCR_CHAT:
      sprintf (s, "encr_chat#%d", tgl_get_peer_id (id));
      break;
    default:
      assert (0);
    }

    assert (json_object_set (res, "print_name", json_string (s)) >= 0);
    return res;
  }
  
  assert (json_object_set (res, "print_name", json_string (P->print_name)) >= 0);
  assert (json_object_set (res, "flags", json_integer (P->flags)) >= 0);
  
  switch (tgl_get_peer_type (id)) {
  case TGL_PEER_USER:
    json_pack_user (res, P);
    break;
  case TGL_PEER_CHAT:
    json_pack_chat (res, P);
    break;
  case TGL_PEER_ENCR_CHAT:
    json_pack_encr_chat (res, P);
    break;
  default:
    assert (0);
  }
  return res;
}

json_t *json_pack_updates (unsigned flags) {
  json_t *a = json_array ();
  
  if (flags & TGL_UPDATE_CREATED) {
    assert (json_array_append (a, json_string ("created")) >= 0);
  }  
  if (flags & TGL_UPDATE_DELETED) {
    assert (json_array_append (a, json_string ("deleted")) >= 0);
  }  
  if (flags & TGL_UPDATE_PHONE) {
    assert (json_array_append (a, json_string ("phone")) >= 0);
  }
  if (flags & TGL_UPDATE_CONTACT) {
    assert (json_array_append (a, json_string ("contact")) >= 0);
  }
  if (flags & TGL_UPDATE_PHOTO) {
    assert (json_array_append (a, json_string ("photo")) >= 0);
  }
  if (flags & TGL_UPDATE_BLOCKED) {
    assert (json_array_append (a, json_string ("blocked")) >= 0);
  }
  if (flags & TGL_UPDATE_REAL_NAME) {
    assert (json_array_append (a, json_string ("real_name")) >= 0);
  }
  if (flags & TGL_UPDATE_NAME) {
    assert (json_array_append (a, json_string ("name")) >= 0);
  }
  if (flags & TGL_UPDATE_REQUESTED) {
    assert (json_array_append (a, json_string ("requested")) >= 0);
  }
  if (flags & TGL_UPDATE_WORKING) {
    assert (json_array_append (a, json_string ("working")) >= 0);
  }
  if (flags & TGL_UPDATE_FLAGS) {
    assert (json_array_append (a, json_string ("flags")) >= 0);
  }
  if (flags & TGL_UPDATE_TITLE) {
    assert (json_array_append (a, json_string ("title")) >= 0);
  }
  if (flags & TGL_UPDATE_ADMIN) {
    assert (json_array_append (a, json_string ("admin")) >= 0);
  }
  if (flags & TGL_UPDATE_MEMBERS) {
    assert (json_array_append (a, json_string ("members")) >= 0);
  }
  if (flags & TGL_UPDATE_USERNAME) {
    assert (json_array_append (a, json_string ("username")) >= 0);
  }

  return a;
}


json_t *json_pack_media (struct tgl_message_media *M) {
  json_t *res = json_object ();

  switch (M->type) {
  case tgl_message_media_photo:
    assert (json_object_set (res, "type", json_string ("photo")) >= 0);
    if (M->caption) {
      assert (json_object_set (res, "caption", json_string (M->caption)) >= 0);
    }
    break;
  case tgl_message_media_document:
  case tgl_message_media_document_encr:
    assert (json_object_set (res, "type", json_string ("document")) >= 0);
    break;
  case tgl_message_media_unsupported:
    assert (json_object_set (res, "type", json_string ("unsupported")) >= 0);
    break;
  case tgl_message_media_geo:
    assert (json_object_set (res, "type", json_string ("geo")) >= 0);
    assert (json_object_set (res, "longitude", json_real (M->geo.longitude)) >= 0);
    assert (json_object_set (res, "latitude", json_real (M->geo.latitude)) >= 0);
    break;
  case tgl_message_media_contact:
    assert (json_object_set (res, "type", json_string ("contact")) >= 0);
    assert (json_object_set (res, "phone", json_string (M->phone)) >= 0);
    assert (json_object_set (res, "first_name", json_string (M->first_name)) >= 0);
    assert (json_object_set (res, "last_name", json_string (M->last_name)) >= 0);
    assert (json_object_set (res, "user_id", json_integer (M->user_id)) >= 0);
    break;
  case tgl_message_media_webpage:
    assert (json_object_set (res, "type", json_string ("webpage")) >= 0);
    if (M->webpage->url) {
      assert (json_object_set (res, "url", json_string (M->webpage->url)) >= 0);
    }
    if (M->webpage->title) {
      assert (json_object_set (res, "title", json_string (M->webpage->title)) >= 0);
    }
    if (M->webpage->description) {
      assert (json_object_set (res, "description", json_string (M->webpage->description)) >= 0);
    }
    if (M->webpage->author) {
      assert (json_object_set (res, "author", json_string (M->webpage->author)) >= 0);
    }
    break;
  case tgl_message_media_venue:
    assert (json_object_set (res, "type", json_string ("venue")) >= 0);
    assert (json_object_set (res, "longitude", json_real (M->venue.geo.longitude)) >= 0);
    assert (json_object_set (res, "latitude", json_real (M->venue.geo.latitude)) >= 0);
    if (M->venue.title) {
      assert (json_object_set (res, "type", json_string (M->venue.title)) >= 0);
    }
    if (M->venue.address) {
      assert (json_object_set (res, "address", json_string (M->venue.address)) >= 0);
    }
    if (M->venue.provider) {
      assert (json_object_set (res, "provider", json_string (M->venue.provider)) >= 0);
    }
    if (M->venue.venue_id) {
      assert (json_object_set (res, "venue_id", json_string (M->venue.venue_id)) >= 0);
    }
    break;
  default:
    assert (json_object_set (res, "type", json_string ("???")) >= 0);
  }
  return res;
}

json_t *json_pack_service (struct tgl_message *M) {
  json_t *res = json_object ();
  switch (M->action.type) {
  case tgl_message_action_geo_chat_create:
    assert (json_object_set (res, "type", json_string ("geo_created")) >= 0);
    break;
  case tgl_message_action_geo_chat_checkin:
    assert (json_object_set (res, "type", json_string ("geo_checkin")) >= 0);
    break;
  case tgl_message_action_chat_create:
    assert (json_object_set (res, "type", json_string ("chat_created")) >= 0);
    assert (json_object_set (res, "title", json_string (M->action.title)) >= 0);
    break;
  case tgl_message_action_chat_edit_title:
    assert (json_object_set (res, "type", json_string ("chat_rename")) >= 0);
    assert (json_object_set (res, "title", json_string (M->action.title)) >= 0);
    break;
  case tgl_message_action_chat_edit_photo:
    assert (json_object_set (res, "type", json_string ("chat_change_photo")) >= 0);
    break;
  case tgl_message_action_chat_delete_photo:
    assert (json_object_set (res, "type", json_string ("chat_delete_photo")) >= 0);
    break;
  case tgl_message_action_chat_add_user:
    assert (json_object_set (res, "type", json_string ("chat_add_user")) >= 0);
    assert (json_object_set (res, "user", json_pack_peer (tgl_set_peer_id (TGL_PEER_USER, M->action.user), tgl_peer_get (TLS, tgl_set_peer_id (TGL_PEER_USER, M->action.user)))) >= 0);
    break;
  case tgl_message_action_chat_add_user_by_link:
    assert (json_object_set (res, "type", json_string ("chat_add_user_link")) >= 0);
    assert (json_object_set (res, "user", json_pack_peer (tgl_set_peer_id (TGL_PEER_USER, M->action.user), tgl_peer_get (TLS, tgl_set_peer_id (TGL_PEER_USER, M->action.user)))) >= 0);
    break;
  case tgl_message_action_chat_delete_user:
    assert (json_object_set (res, "type", json_string ("chat_del_user")) >= 0);
    assert (json_object_set (res, "user", json_pack_peer (tgl_set_peer_id (TGL_PEER_USER, M->action.user), tgl_peer_get (TLS, tgl_set_peer_id (TGL_PEER_USER, M->action.user)))) >= 0);
    break;
  case tgl_message_action_set_message_ttl:
    assert (json_object_set (res, "type", json_string ("set_ttl")) >= 0);
    assert (json_object_set (res, "ttl", json_integer (M->action.ttl)) >= 0);
    break;
  case tgl_message_action_read_messages:
    assert (json_object_set (res, "type", json_string ("read")) >= 0);
    assert (json_object_set (res, "count", json_integer (M->action.read_cnt)) >= 0);
    break;
  case tgl_message_action_delete_messages:
    assert (json_object_set (res, "type", json_string ("delete")) >= 0);
    assert (json_object_set (res, "count", json_integer (M->action.delete_cnt)) >= 0);
    break;
  case tgl_message_action_screenshot_messages:
    assert (json_object_set (res, "type", json_string ("screenshot")) >= 0);
    assert (json_object_set (res, "count", json_integer (M->action.screenshot_cnt)) >= 0);
    break;
  case tgl_message_action_flush_history:
    assert (json_object_set (res, "type", json_string ("flush")) >= 0);
    break;
  case tgl_message_action_resend:
    assert (json_object_set (res, "type", json_string ("resend")) >= 0);
    break;
  case tgl_message_action_notify_layer:
    assert (json_object_set (res, "type", json_string ("notify_layer")) >= 0);
    assert (json_object_set (res, "layer", json_integer (M->action.layer)) >= 0);
    break;
  case tgl_message_action_typing:    
    assert (json_object_set (res, "type", json_string ("typing")) >= 0);
    break;
  case tgl_message_action_noop:
    assert (json_object_set (res, "type", json_string ("noop")) >= 0);
    break;
  case tgl_message_action_request_key:
    assert (json_object_set (res, "type", json_string ("request_key")) >= 0);
    break;
  case tgl_message_action_accept_key:
    assert (json_object_set (res, "type", json_string ("accept_key")) >= 0);
    break;
  case tgl_message_action_commit_key:
    assert (json_object_set (res, "type", json_string ("commit_key")) >= 0);
    break;
  case tgl_message_action_abort_key:
    assert (json_object_set (res, "type", json_string ("abort_key")) >= 0);
    break;
  default:
    assert (json_object_set (res, "type", json_string ("???")) >= 0);
    break;
  }
  return res;
}

json_t *json_pack_message (struct tgl_message *M) {  
  json_t *res = json_object ();
  assert (json_object_set (res, "event", json_string ("message")) >= 0);
  //will overwriten to service, if service.

  assert (json_object_set (res, "id", json_integer (M->id)) >= 0);
  if (!(M->flags & TGLMF_CREATED)) { return res; }

  assert (json_object_set (res, "flags", json_integer (M->flags)) >= 0);
 
  if (tgl_get_peer_type (M->fwd_from_id)) {
    assert (json_object_set (res, "fwd_from", json_pack_peer (M->fwd_from_id, tgl_peer_get (TLS, M->fwd_from_id))) >= 0);
    assert (json_object_set (res, "fwd_date", json_integer (M->fwd_date)) >= 0);
  }

  if (M->reply_id) {
    assert (json_object_set (res, "reply_id", json_integer (M->reply_id)) >= 0);
  }

  if (M->flags & TGLMF_MENTION) {
    assert (json_object_set (res, "mention", json_true ()) >= 0);
  }
 
  assert (json_object_set (res, "from", json_pack_peer (M->from_id, tgl_peer_get (TLS, M->from_id))) >= 0);
  assert (json_object_set (res, "to", json_pack_peer (M->to_id, tgl_peer_get (TLS, M->to_id))) >= 0);
  
  assert (json_object_set (res, "out", json_boolean (M->flags & TGLMF_OUT)) >= 0);
  assert (json_object_set (res, "unread", json_boolean (M->flags & TGLMF_UNREAD)) >= 0);
  assert (json_object_set (res, "service", json_boolean (M->flags & TGLMF_SERVICE)) >= 0);
  assert (json_object_set (res, "date", json_integer (M->date)) >= 0);
  
  if (!(M->flags & TGLMF_SERVICE)) {  
    if (M->message_len && M->message) {
      assert (json_object_set (res, "text", json_string (M->message)) >= 0);
    }
    if (M->media.type && M->media.type != tgl_message_media_none) {
      assert (json_object_set (res, "media", json_pack_media (&M->media)) >= 0);
    }
  } else {
    assert (json_object_set (res, "event", json_string ("service")) >= 0);
    assert (json_object_set (res, "action", json_pack_service (M)) >= 0);
  }
  return res;
}

json_t *json_pack_read (struct tgl_message *M) {
  json_t *res = json_pack_message (M);
  assert (json_object_set (res, "event", json_string ("read")) >= 0);
  //this will overwrite "event":"message" to "event":"read".
  return res;
}

#endif