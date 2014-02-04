#ifndef __SPOTIFY_PARSER_H__
#define __SPOTIFY_PARSER_H__

void result_cleanup(gpointer);
GSList *get_results(char *, UnityCancellable *);
static const guint MAX_RESULTS = 5;

/**
 * This is just an example result type with some sample
 * fields. You should modify the fields to match the
 * data you are expecting from your search source
 */
typedef struct {
  gchar *link;
  gchar *title;
  gchar *icon_url;
  gchar *popularity;
  gchar *n_of_albums;
} result_t;

#endif /* __SPOTIFY_PARSER_H__ */
