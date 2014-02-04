/*
 * Json parser backend
 *
 */
#include <json-glib/json-glib.h>
#include <unity.h>
#include "spotify-parser.h"


/*
 * This is the URI against which you'll be submitting your
 * search query from the Dash.
 * http://stackoverflow.com/questions/16081068/spotify-metadata-api-search-by-artist
 */
#define ARTIST_BASE_URI "http://ws.spotify.com/search/1/artist.json?q=artist:"
#define ARTIST_LOOKUP_URI "http://ws.spotify.com/lookup/1/.json?uri=&extras=album"
#define ARTIST_ICON_BASE_URI  "https://embed.spotify.com/oembed/?url="


/**
 * @brief Cleans up (frees the memory allocated memory)
 *        of the given results structure
 * @param data Structure that contains the data to free
 */
void result_cleanup(gpointer data) {
  result_t *result = (result_t *)data;
  if (result->link)
    free(result->link);
  if (result->icon_url)
    free(result->icon_url);
  if (result->title)
    free(result->title);
  if (result->popularity)
    free(result->popularity);
  if (result->n_of_albums)
    free(result->n_of_albums);
}


/**
 * @brief remove extra whitespaces from search string
 * @param search string to modify
 */
void trim_string(char *string){
  int length = strlen(string);
  int index, index_alt;  

  while ( string[0] == ' '){
    for (index = 0; index < length; index++)
      string[index] = string[index+1];
    length--;
  }

  while ( string[length - 1] == ' ' ){
    string[length - 1] = '\0';
    length--;
  }

  for (index = 0; index < length -1; index++){
    while ( string[index] == ' ' && string[index + 1] == ' ' ){
      for (index_alt = index + 1; index_alt < length; index_alt++)
	string[index_alt] = string[index_alt+1];
      length--;
    }
  }
}


/**
 * @brief get_spotify_thumbnail Get the Spotify artist thumbnail url
 * @param spotify_uri Spotify uri for the artist
 * @return Pointer to the string containing the Spotify artist thumbnail url
 */
char * get_spotify_thumbnail(const char *spotify_uri){
  GString *url = g_string_new(ARTIST_ICON_BASE_URI);
  g_string_append(url, spotify_uri);

  GError *error = NULL;
  JsonParser *parser = json_parser_new(); 
  GFile * file = g_file_new_for_uri(url->str);

  g_print("Opening %s for reading\n", url->str);
  GInputStream * fis = (GInputStream* ) g_file_read(file, NULL, &error);
  g_print("Opened!\n");
  if (error){
    g_debug("** ERROR **: %s (domain: %s, code: %d) at %d (in get_spotify_thumbnail)\n",\
	     error->message, g_quark_to_string (error->domain), error->code, \
	     __LINE__);
    g_object_unref(file);
    g_object_unref(fis);
    g_object_unref (parser);
    g_string_free(url, TRUE);
    return NULL;
  }

  json_parser_load_from_stream(parser, fis, NULL, &error);
  if (error){
    g_debug("Unable to parse `%s': %s\n", url->str, error->message);
    g_object_unref(file);
    g_object_unref(fis);
    g_object_unref (parser);
    g_string_free(url, TRUE);
    return NULL;
  }

  JsonNode *root = json_parser_get_root(parser);
  JsonObject * content = json_node_get_object(root);
  const gchar * thumbnail_url = \
    json_object_get_string_member(content, "thumbnail_url");
  gchar * pointer = (gchar *) malloc( strlen(thumbnail_url) + 1 );
  char ** tokens = g_strsplit(thumbnail_url, "cover", 0);
  if ( *(tokens + 1) ){
    gchar * new_thumbnail_url = g_strconcat(*tokens, "640", \
					    *(tokens+1), '\0', NULL);
    g_strfreev(tokens);
    strcpy(pointer, new_thumbnail_url);
    g_free(new_thumbnail_url);
  }
  else{
    strcpy(pointer, thumbnail_url);
  }

  g_object_unref(file);
  g_object_unref(fis);
  g_object_unref(parser);
  g_string_free(url, TRUE);
  return pointer;
}


/**
 * @brief get_spotify_artist_albums Get the number of artist albums on Spotify
 * @param spotify_uri Spotify uri for the artist
 * @return Number of number of artist albums on Spotify
 */
guint get_spotify_artist_albums(const gchar *spotify_uri){
  GString *url = g_string_new(ARTIST_LOOKUP_URI);
  url = g_string_insert(url, 41, spotify_uri);

  GError *error = NULL;
  JsonParser *parser = json_parser_new(); 
  GFile * file = g_file_new_for_uri(url->str);

  g_print("Opening %s for reading\n", url->str);
  GInputStream * fis = (GInputStream*)g_file_read (file, NULL, &error);
  g_print("Opened!\n");

  if (error){
    g_debug("** ERROR **: %s (domain: %s, code: %d) at %d (in get_spotify_artist_albums)\n", \
	     error->message, g_quark_to_string (error->domain), error->code, \
	     __LINE__);
    g_object_unref(file);
    g_object_unref(fis);
    g_object_unref (parser);
    g_string_free(url, TRUE);
    return 0;
  }

  json_parser_load_from_stream(parser, fis, NULL, &error);
  if (error){
    g_debug("Unable to parse `%s': %s\n", url->str, error->message);
    g_object_unref(file);
    g_object_unref(fis);
    g_object_unref (parser);
    g_string_free(url, TRUE);
    return 0;
  }

  JsonNode *root = json_parser_get_root(parser);
  JsonObject * content = json_node_get_object(root);
  JsonNode * node = json_object_get_member(content, "artist");
  content = json_node_get_object(node);
  node = json_object_get_member(content, "albums");
  JsonArray * AlbumsArray = json_node_get_array(node);
  guint AlbumsArrayLength = json_array_get_length(AlbumsArray);

  g_object_unref(file);
  g_object_unref(fis);
  g_object_unref (parser);
  g_string_free(url, TRUE);
  return AlbumsArrayLength;
}


/**
 * @brief get_results Get and parse the results from a search query
 * @param search_term String submitted as the search term
 * @return Search results
 */
GSList * get_results(char *search_term, UnityCancellable* cancellable){
  GString *url = NULL;
  GSList *results = NULL;
  result_t *result = NULL;

  trim_string(search_term);
  /* Check if a proper search term was submitted, return otherwise */
  if ( search_term == NULL || search_term[0] == '\0' || strlen(search_term) < 3 ) {
    g_warning("get_results: search_term null or too short\n");
    return results;
  }

  g_warning("Starting search for %s\n", search_term);

  /* Construct the full search query */
  url = g_string_new(ARTIST_BASE_URI);
  g_string_append(url, search_term);

  GError *error = NULL;
  JsonParser *parser = json_parser_new();
  GFile * file = g_file_new_for_uri(url->str);

  g_print("\nReading %s from file pointer...", url->str);
  GInputStream * fis = (GInputStream*) \
    g_file_read(file, unity_cancellable_get_gcancellable(cancellable), &error);
  g_print("done!\n");

  if (error){
    g_debug("** ERROR **: %s (domain: %s, code: %d) at %d (in get_results) \n",\
	    error->message, g_quark_to_string (error->domain), error->code, \
	    __LINE__);
    g_object_unref(file);
    if (fis)
      g_object_unref(fis);
    g_object_unref (parser);
    g_string_free(url, TRUE);
    return results;
  }

  json_parser_load_from_stream(parser, fis, NULL, &error);
  if (error){
    g_debug("Unable to parse `%s': %s\n", url->str, error->message);
    g_object_unref(file);
    g_object_unref(fis);
    g_object_unref (parser);
    g_string_free(url, TRUE);
    return results;
  }

  JsonNode *root = json_parser_get_root(parser);
  JsonObject * content = json_node_get_object(root);

  JsonNode * ArtistsNode = json_object_get_member(content, "artists");
  JsonArray * ArtistsArray = json_node_get_array(ArtistsNode);
  guint ArtistsArrayLength = json_array_get_length(ArtistsArray);
  if ( ArtistsArrayLength > MAX_RESULTS)
    ArtistsArrayLength = MAX_RESULTS;
  guint i = 0;
  for (i = 0; i < ArtistsArrayLength; i++){
    if ( g_cancellable_is_cancelled( unity_cancellable_get_gcancellable( cancellable ) ) )
      break;
    JsonNode * artistNode = json_array_get_element(ArtistsArray, i);
    JsonObject * artistNodeContent = json_node_get_object(artistNode);
    const gchar * title = json_object_get_string_member(artistNodeContent, "name");
    const gchar * spotify_uri = \
      json_object_get_string_member(artistNodeContent, "href");
    const gchar * popularity = \
      json_object_get_string_member(artistNodeContent, "popularity");

    gchar * n_of_albums = g_strdup_printf("%i", get_spotify_artist_albums(spotify_uri) );

    result = (result_t*)malloc(sizeof(result_t));
    bzero((result_t*)result, sizeof(result_t));

    gchar * fallback_icon_url = \
      "/usr/share/icons/unity-icon-theme/places/svg/service-spotify.svg";    
    g_print("Fetching thumbnail for %s\n", title);
    gchar * icon_url = get_spotify_thumbnail(spotify_uri);
    if ( icon_url == NULL ) {
      icon_url = fallback_icon_url;
      result->icon_url = (char *)malloc(strlen(icon_url)+1);
      strcpy(result->icon_url, icon_url);
    }
    else {
      result->icon_url = (char *)malloc(strlen(icon_url)+1);
      strcpy(result->icon_url, icon_url);
      free(icon_url);
    }

    g_print("%s\n\n", result->icon_url);

    result->title = (char *)malloc(strlen(title)+1);
    strcpy(result->title, title);

    result->link = (char *)malloc(strlen(spotify_uri)+1);
    strcpy(result->link, spotify_uri);

    result->popularity = (char *)malloc(strlen(popularity)+1);
    strcpy(result->popularity, popularity);

    result->n_of_albums = (char *)malloc(strlen(n_of_albums)+1);
    strcpy(result->n_of_albums, n_of_albums);
    g_free(n_of_albums);

    results = g_slist_append(results, result);
  }

  g_object_unref(file);
  g_object_unref(fis);
  g_object_unref (parser);
  g_string_free(url, TRUE);
  return results;
}
