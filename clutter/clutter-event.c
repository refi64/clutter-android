/*
 * Clutter.
 *
 * An OpenGL based 'interactive canvas' library.
 *
 * Authored By Matthew Allum  <mallum@openedhand.com>
 *
 * Copyright (C) 2006 OpenedHand
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "clutter-keysyms-table.h"
#include "clutter-event.h"
#include "clutter-private.h"
#include "clutter-debug.h"

/* main event handler */
ClutterEventFunc _clutter_event_func    = NULL;
gpointer         _clutter_event_data    = NULL;
GDestroyNotify   _clutter_event_destroy = NULL;

/**
 * clutter_event_type:
 * @event: a #ClutterEvent
 *
 * Retrieves the type of the event.
 *
 * Return value: a #ClutterEventType
 */
ClutterEventType
clutter_event_type (ClutterEvent *event)
{
  g_return_val_if_fail (event != NULL, CLUTTER_NOTHING);

  return event->type;
}

/**
 * clutter_event_get_time:
 * @event: a #ClutterEvent
 *
 * Retrieves the time of the event.
 *
 * Return value: the time of the event, or %CLUTTER_CURRENT_TIME
 *
 * Since: 0.4
 */
guint32
clutter_event_get_time (ClutterEvent *event)
{
  g_return_val_if_fail (event != NULL, CLUTTER_CURRENT_TIME);

  switch (event->type)
    {
    case CLUTTER_KEY_PRESS:
    case CLUTTER_KEY_RELEASE:
      return event->key.time;
    case CLUTTER_BUTTON_PRESS:
    case CLUTTER_2BUTTON_PRESS:
    case CLUTTER_BUTTON_RELEASE:
      return event->button.time;
    case CLUTTER_MOTION:
      return event->motion.time;
    case CLUTTER_SCROLL:
      return event->scroll.time;
    default:
      break;
    }

  return CLUTTER_CURRENT_TIME;
}

/**
 * clutter_event_get_state:
 * @event: a #ClutterEvent
 *
 * Retrieves the modifier state of the event.
 *
 * Return value the modifier state parameter, or 0
 *
 * Since: 0.4
 */
guint32
clutter_event_get_state (ClutterEvent *event)
{
  g_return_val_if_fail (event != NULL, 0);

  switch (event->type)
    {
    case CLUTTER_KEY_PRESS:
    case CLUTTER_KEY_RELEASE:
      return event->key.modifier_state;
    case CLUTTER_BUTTON_PRESS:
    case CLUTTER_2BUTTON_PRESS:
    case CLUTTER_BUTTON_RELEASE:
      return event->button.modifier_state;
    case CLUTTER_MOTION:
      return event->motion.modifier_state;
    case CLUTTER_SCROLL:
      return event->scroll.modifier_state;
    default:
      break;
    }

  return 0;
}

/**
 * clutter_event_get_coords:
 * @event: a #ClutterEvent
 * @x: return location for the X coordinate
 * @y: return location for the Y coordinate
 *
 * Retrieves the coordinates of @event and puts them into @x and @y.
 *
 * Since: 0.4
 */
void
clutter_event_get_coords (ClutterEvent *event,
                          gint         *x,
                          gint         *y)
{
  gint event_x, event_y;

  g_return_if_fail (event != NULL);

  event_x = event_y = 0;

  switch (event->type)
    {
    case CLUTTER_KEY_PRESS:
    case CLUTTER_KEY_RELEASE:
      event_x = event_y = 0;
      break;
    case CLUTTER_BUTTON_PRESS:
    case CLUTTER_2BUTTON_PRESS:
    case CLUTTER_BUTTON_RELEASE:
      event_x = event->button.x;
      event_y = event->button.y;
      break;
    case CLUTTER_MOTION:
      event_x = event->motion.x;
      event_y = event->motion.y;
      break;
    case CLUTTER_SCROLL:
      event_x = event->scroll.x;
      event_y = event->scroll.y;
      break;
    default:
      break;
    }

  if (x)
    *x = event_x;

  if (y)
    *y = event_y;
}

/**
 * clutter_button_event_button:
 * @buttev: a #ClutterButtonEvent
 *
 * Retrieve the button number of the event.
 *
 * Return value: the button number.
 *
 * Since: 0.4
 */
guint32
clutter_button_event_button (ClutterButtonEvent *buttev)
{
  g_return_val_if_fail (buttev != NULL, 0);

  return buttev->button;
}

/* keys */

/**
 * clutter_key_event_symbol:
 * @keyev: A #ClutterKeyEvent
 *
 * Retrieves the value of the key that caused @keyev.
 *
 * Return value: The keysym representing the key
 */
guint
clutter_key_event_symbol (ClutterKeyEvent *keyev)
{
  g_return_val_if_fail (keyev != NULL, 0);

  return keyev->keyval;
}

/**
 * clutter_key_event_code:
 * @keyev: A #ClutterKeyEvent
 *
 * Retrieves the keycode of the key that caused @keyev.
 *
 * Return value: The keycode representing the key
 */
guint16
clutter_key_event_code (ClutterKeyEvent *keyev)
{
  g_return_val_if_fail (keyev != NULL, 0);

  return keyev->hardware_keycode;
}

/**
 * clutter_key_event_unicode:
 * @keyev: A #ClutterKeyEvent
 *
 * Retrieves the unicode value for the key that caused @keyev.
 *
 * Return value: The unicode value representing the key
 */
guint32
clutter_key_event_unicode (ClutterKeyEvent *keyev)
{
  g_return_val_if_fail (keyev != NULL, 0);
  
  return clutter_keysym_to_unicode (keyev->keyval);
}

/**
 * clutter_keysym_to_unicode:
 * @keyval: a clutter key symbol 
 * 
 * Convert from a GDK key symbol to the corresponding ISO10646 (Unicode)
 * character.
 * 
 * Return value: the corresponding unicode character, or 0 if there
 *               is no corresponding character.
 **/
guint32
clutter_keysym_to_unicode (guint keyval)
{
  int min = 0;
  int max = G_N_ELEMENTS (clutter_keysym_to_unicode_tab) - 1;
  int mid;

  /* First check for Latin-1 characters (1:1 mapping) */
  if ((keyval >= 0x0020 && keyval <= 0x007e) ||
      (keyval >= 0x00a0 && keyval <= 0x00ff))
    return keyval;

  /* Also check for directly encoded 24-bit UCS characters:
   */
  if ((keyval & 0xff000000) == 0x01000000)
    return keyval & 0x00ffffff;

  /* binary search in table */
  while (max >= min) {
    mid = (min + max) / 2;
    if (clutter_keysym_to_unicode_tab[mid].keysym < keyval)
      min = mid + 1;
    else if (clutter_keysym_to_unicode_tab[mid].keysym > keyval)
      max = mid - 1;
    else {
      /* found it */
      return clutter_keysym_to_unicode_tab[mid].ucs;
    }
  }
 
  /* No matching Unicode value found */
  return 0;
}

GType
clutter_event_get_type (void)
{
  static GType our_type = 0;

  if (!our_type)
    our_type = g_boxed_type_register_static ("ClutterEvent",
		    			     (GBoxedCopyFunc) clutter_event_copy,
					     (GBoxedFreeFunc) clutter_event_free);
  return our_type;
}

static GHashTable *event_hash = NULL;

/**
 * clutter_event_new:
 * @type: The type of event.
 *
 * Creates a new #ClutterEvent of the specified type.
 *
 * Return value: A newly allocated #ClutterEvent.
 */
ClutterEvent *
clutter_event_new (ClutterEventType type)
{
  ClutterEvent *new_event;

  if (!event_hash)
    event_hash = g_hash_table_new (g_direct_hash, NULL);

  new_event = g_slice_new0 (ClutterEvent);

  new_event->type = new_event->any.type = type;
  new_event->flags = CLUTTER_EVENT_NONE;

  g_hash_table_insert (event_hash, new_event, GUINT_TO_POINTER (1));

  return new_event;
}

/**
 * clutter_event_copy:
 * @event: A #ClutterEvent.
 *
 * Copies @event.
 *
 * Return value: A newly allocated #ClutterEvent
 */
ClutterEvent *
clutter_event_copy (ClutterEvent *event)
{
  ClutterEvent *new_event;

  g_return_val_if_fail (event != NULL, NULL);

  new_event = clutter_event_new (CLUTTER_NOTHING);
  *new_event = *event;

  return new_event;
}

/**
 * clutter_event_free:
 * @event: A #ClutterEvent.
 *
 * Frees all resources used by @event.
 */
void
clutter_event_free (ClutterEvent *event)
{
  if (G_LIKELY (event))
    {
      g_hash_table_remove (event_hash, event);
      g_slice_free (ClutterEvent, event);
    }
}

/**
 * clutter_event_get:
 *
 * FIXME
 *
 * Return value: FIXME
 *
 * Since: 0.4
 */
ClutterEvent *
clutter_event_get (void)
{
  ClutterMainContext *context = clutter_context_get_default ();
  ClutterBackend *backend = context->backend;

  _clutter_events_queue (backend);
  return _clutter_event_queue_pop (backend);
}

/**
 * clutter_event_peek:
 * 
 * FIXME
 *
 * Return value: FIXME
 *
 * Since: 0.4
 */
ClutterEvent *
clutter_event_peek (void)
{
  ClutterMainContext *context = clutter_context_get_default ();
  
  return _clutter_event_queue_peek (context->backend);
}

/**
 * clutter_event_put:
 * @event: a #ClutterEvent
 *
 * FIXME
 *
 * Since: 0.4
 */
void
clutter_event_put (ClutterEvent *event)
{
  ClutterMainContext *context = clutter_context_get_default ();
  ClutterBackend *backend = context->backend;

  _clutter_event_queue_push (backend, clutter_event_copy (event));
}

void
_clutter_event_queue_push (ClutterBackend *backend,
                           ClutterEvent   *event)
{
  if (!backend->events_queue)
    backend->events_queue = g_queue_new ();

  g_queue_push_head (backend->events_queue, event);
}

ClutterEvent *
_clutter_event_queue_pop (ClutterBackend *backend)
{
  if (!backend->events_queue)
    return NULL;

  return g_queue_pop_tail (backend->events_queue);
}

ClutterEvent *
_clutter_event_queue_peek (ClutterBackend *backend)
{
  if (!backend->events_queue)
    return NULL;

  return g_queue_peek_tail (backend->events_queue);
}

gboolean
_clutter_event_queue_check_pending (ClutterBackend *backend)
{
  if (!backend->events_queue)
    return FALSE;

  return g_queue_is_empty (backend->events_queue) == FALSE;
}

void
_clutter_set_events_handler (ClutterEventFunc func,
                             gpointer         data,
                             GDestroyNotify   destroy)
{
  if (_clutter_event_destroy)
    (* _clutter_event_destroy) (_clutter_event_data);

  _clutter_event_func = func;
  _clutter_event_data = data;
  _clutter_event_destroy = destroy;
}

void
_clutter_synthetise_stage_state (ClutterBackend    *backend,
                                 ClutterEvent      *event,
                                 ClutterStageState  set_flags,
                                 ClutterStageState  unset_flags)
{

}

void
_clutter_synthesize_click (ClutterBackend *backend,
                           ClutterEvent   *event,
                           gint            n_clicks)
{
  ClutterEvent temp_event;

  temp_event = *event;
  temp_event.type = (n_clicks == 2) ? CLUTTER_2BUTTON_PRESS
                                    : CLUTTER_3BUTTON_PRESS;

  clutter_backend_put_event (backend, &temp_event);
}

void
_clutter_event_button_generate (ClutterBackend *backend,
                                ClutterEvent   *event)
{
  if ((event->button.time < (backend->button_click_time[1] + 2 * backend->double_click_time)) &&
      (event->button.button == backend->button_number[1]) &&
      (ABS (event->button.x - backend->button_x[1]) <= backend->double_click_distance) &&
      (ABS (event->button.y - backend->button_y[1]) <= backend->double_click_distance))
    {
      _clutter_synthesize_click (backend, event, 3);
            
      backend->button_click_time[1] = 0;
      backend->button_click_time[0] = 0;
      backend->button_number[1] = -1;
      backend->button_number[0] = -1;
      backend->button_x[0] = backend->button_x[1] = 0;
      backend->button_y[0] = backend->button_y[1] = 0;
    }
  else if ((event->button.time < (backend->button_click_time[0] + backend->double_click_time)) &&
      (event->button.button == backend->button_number[0]) &&
      (ABS (event->button.x - backend->button_x[0]) <= backend->double_click_distance) &&
      (ABS (event->button.y - backend->button_y[0]) <= backend->double_click_distance))
    {
      _clutter_synthesize_click (backend, event, 2);
      
      backend->button_click_time[1] = backend->button_click_time[0];
      backend->button_click_time[0] = event->button.time;
      backend->button_number[1] = backend->button_number[0];
      backend->button_number[0] = event->button.button;
      backend->button_x[1] = backend->button_x[0];
      backend->button_x[0] = event->button.x;
      backend->button_y[1] = backend->button_y[0];
      backend->button_y[0] = event->button.y;
    }
  else
    {
      backend->button_click_time[1] = 0;
      backend->button_click_time[0] = event->button.time;
      backend->button_number[1] = -1;
      backend->button_number[0] = event->button.button;
      backend->button_x[1] = 0;
      backend->button_x[0] = event->button.x;
      backend->button_y[1] = 0;
      backend->button_y[0] = event->button.y;
    }
}
