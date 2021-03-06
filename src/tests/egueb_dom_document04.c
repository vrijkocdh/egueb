/* be sure that a document is correctly set on a tree */
#include "egueb_dom_mylib.h"
#include <stdio.h>

static void help(void)
{
	printf("Usage: egueb_dom_document04 FILE.xml\n");
	exit(1);
}

int main(int argc, char **argv)
{
	Egueb_Dom_Node *doc, *child, *root;
	Enesim_Stream *im;

	if (argc < 2) help();

	mylib_init();
	doc = mydocument_new();
	im = enesim_stream_file_new(argv[1], "r+");
	if (!im) return -1;

	egueb_dom_parser_parse(im, &doc);

	printf("[testing] get the topmost element and destroy the document\n");
	root = egueb_dom_document_document_element_get(doc);
	egueb_dom_node_unref(doc);
	doc = NULL;

	printf("[testing] get the document of the topmost\n");
	doc = egueb_dom_node_owner_document_get(root);
	printf("[testing] doc = %p\n", doc);
	
	printf("[testing] get the document of a child\n");
	child = egueb_dom_node_child_first_get(root);
	if (child)
	{
		doc = egueb_dom_node_owner_document_get(child);
		printf("[testing] doc = %p\n", doc);
		egueb_dom_node_unref(child);
	}
	egueb_dom_node_unref(root);

	/* now test the dangling nodes */
	doc = mydocument_new();
	child = myelement_new();
	child = egueb_dom_document_node_adopt(doc, child, NULL);
	egueb_dom_node_unref(doc);

	printf("[testing] get the document of dangling node\n");
	doc = egueb_dom_node_owner_document_get(child);
	printf("[testing] doc = %p\n", doc);

	return 0;
}

