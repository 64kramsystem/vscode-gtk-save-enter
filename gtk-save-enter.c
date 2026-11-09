/* Restore GTK's standard Save response in Chromium's native file chooser. */
#include <gtk/gtk.h>
#include <gmodule.h>

/* Chromium's custom response: https://github.com/electron/electron/issues/54670 */
static const gint chromium_accept_response = 0;

static gboolean restore_save_response(GSignalInvocationHint *hint,
		guint count, const GValue *values, gpointer data)
{
	(void)hint;
	(void)data;
	if (count == 0) {
		return TRUE;
	}
	GObject *object = g_value_get_object(&values[0]);
	if (!GTK_IS_FILE_CHOOSER_DIALOG(object)
			|| gtk_file_chooser_get_action(GTK_FILE_CHOOSER(object)) != GTK_FILE_CHOOSER_ACTION_SAVE) {
		return TRUE;
	}

	GtkDialog *dialog = GTK_DIALOG(object);
	GtkWidget *old_button = gtk_dialog_get_widget_for_response(dialog, chromium_accept_response);
	if (!GTK_IS_BUTTON(old_button)
			|| gtk_dialog_get_widget_for_response(dialog, GTK_RESPONSE_ACCEPT) != NULL) {
		return TRUE;
	}

	/* Recreate the button through GTK's public API so response validation,
	 * filename activation, and overwrite confirmation all use the stock ID. */
	const gchar *button_label = gtk_button_get_label(GTK_BUTTON(old_button));
	if (button_label == NULL) {
		return TRUE;
	}
	gchar *label = g_strdup(button_label);
	gboolean sensitive = gtk_widget_get_sensitive(old_button);
	gtk_widget_destroy(old_button);
	GtkWidget *button = gtk_dialog_add_button(dialog, label, GTK_RESPONSE_ACCEPT);
	g_free(label);
	gtk_widget_set_sensitive(button, sensitive);
	gtk_dialog_set_default_response(dialog, GTK_RESPONSE_ACCEPT);
	gtk_widget_show(button);
	if (g_getenv("VSCODE_GTK_SAVE_DEBUG") != NULL) {
		g_printerr("gtk-save-enter: restored Save As default response\n");
	}
	return TRUE;
}

G_MODULE_EXPORT void gtk_module_init(gint *argc, gchar ***argv)
{
	(void)argc;
	(void)argv;
	/* GTK3_MODULES reaches child applications too; leave their dialogs alone. */
	gchar *executable = g_file_read_link("/proc/self/exe", NULL);
	gboolean is_code = executable != NULL && g_str_has_suffix(executable, "/code");
	g_free(executable);
	if (!is_code) {
		return;
	}
	gpointer widget_class = g_type_class_ref(GTK_TYPE_WIDGET);
	guint signal = g_signal_lookup("map", GTK_TYPE_WIDGET);
	if (signal != 0) {
		g_signal_add_emission_hook(signal, 0, restore_save_response, NULL, NULL);
	}
	g_type_class_unref(widget_class);
}
