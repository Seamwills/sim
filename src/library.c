#include "library.h"

/* linked list function, with sentinel included */
node *
list_init(void) {
    node *sen;

    sen = malloc(sizeof(struct node));
    sen->data = NULL;
    sen->prev = sen;
    sen->next = sen;

    return sen;
}

/* add new element to list sen, use malloc to allocate space for data */
void
list_add(node *sen, type *data)
{
    node *element;

    element = malloc(sizeof(struct node));
    element->data = data;

    element->next = sen->next;
    sen->next->prev = element;
    sen->next = element;
    element->prev = sen;
}

/* free the list sen and its children */
void
list_free(node *sen)
{

    position p, t;
    p = sen->next;
    while (p != sen) {
        t = p;
        p = p->next;
        free(t->data->date);
        free(t->data->title);
        free(t->data);
        free(t);
    }

}

/* quick sort */
void
quicksort(position p, position r)
{
    if (p != r && p != r->next && r != p->prev) {
        position q = partition(p, r);
        quicksort(p, q->prev);
        quicksort(q->next, r);
    }
}

position
partition(position p, position r)
{
    position x, i, j;

    x = r, i = p->prev;
    j = p;
    while (j != r) {
        if (date_cmp(j->data->date, x->data->date) <= 0) {
            i = i->next;
            exchange(i, j);
        }
        j = j->next;
    }
    exchange(i->next, r);
    return i->next;
}

void
exchange(position x, position y)
{
    type data;

    data = *x->data;
    *x->data = *y->data;
    *y->data = data;
}

/* replace sub in line with str, then return pointer of the new string */
char *
str_rep(const char *line, const char *sub, const char *str)
{
    char    *l, *s, *vacant, *t;
    char    *ret;
    size_t  i = 0;

    /* no <h1> title present */
    if (str == NULL) {
        ret = malloc(strlen(line) - strlen(sub) + strlen(VACANT) + 1);
        vacant = malloc(strlen(VACANT) + 1);
        strcpy(vacant, VACANT);
        t = vacant;
    } else {
        ret = malloc(strlen(line) - strlen(sub) + strlen(str) + 1);
        t = (char *) str;
    }
    
    l = (char *) line;
    s = strstr(line, sub);
    while (l + i != s) {
        ret[i] = l[i];
        i++;
    }

    while (*t != '\0')
        ret[i++] = *t++;

    s += strlen(sub);
    while (*s != '\0')
        ret[i++] = *s++;
    ret[i] = '\0';

    return ret;
}

/* sort date */
int
date_cmp(const void *x, const void *y)
{
    const char  *p, *q;
    int     i;

    p = (char *) x, q = (char *) y;
    for (i = 0; i < 10; i++) {
        if (p[i] > q[i])
            return 1;
        if (p[i] < q[i])
            return -1;
    }

    /* if get here, the first 10 bit is the same */
    if (strlen(p) < strlen(q))
        return -1;
    else
        return 1;

    return 0;
}

void
date_sort(list sen)
{
    quicksort(sen->next, sen->prev);
}

/* file process */
int
do_file(char *name, list sen)
{
    char *outline, line[MAXLINE+1];
    char infile[256], outfile[256], template[256];
	FILE *out, *in;
    unsigned int extensions;
    hoedown_buffer *buf;
    type *pair;

    pair = malloc(sizeof(type));
    pair->date = get_date(name);

    sprintf(infile, "markdown/%s", name);
    sprintf(outfile, "site/%s.html", pair->date);
    sprintf(template, ".sim/article.html");

    extensions = HOEDOWN_EXT_FENCED_CODE | HOEDOWN_EXT_TABLES | HOEDOWN_EXT_FOOTNOTES;
    buf = hoedown(infile, extensions);

    /* get article title */
    pair->title = get_title(buf);
    list_add(sen, pair);

	/* writing the result to file */
    out = fopen(outfile, "w");
    if ((in = fopen(template, "r")) == NULL) {
        perror("template open error");
        return 1;
    }

    while (fgets(line, MAXLINE, in) != NULL) {
        if (strstr(line, "\%title\%") != NULL) {
            outline = str_rep(line, "\%title\%", pair->title);
            fputs(outline, out);
            free(outline);
            continue;
        } else if (strstr(line, "\%content\%") != NULL) {
            fwrite(buf->data, 1, buf->size, out);
            continue;
        } else
            fputs(line, out);
    }

	if (ferror(out)) {
		fprintf(stderr, "I/O errors found while writing output.\n");
		return 1;
	}

    fclose(out);
    fclose(in);

    return 0;
}

hoedown_buffer *
hoedown(char *file, unsigned int extensions)
{
	FILE *in;
	hoedown_buffer *ib, *ob;

	hoedown_renderer *renderer;
	hoedown_document *document;

	/* reading everything */
    in = fopen(file, "r");
	ib = hoedown_buffer_new(IUNIT);
	if (!ib) {
		fprintf(stderr, "Couldn't allocate input buffer.\n");
		exit(1);
	}

	while (!feof(in)) {
		if (ferror(in)) {
			fprintf(stderr, "I/O errors found while reading input.\n");
            exit(1);
		}
		if (hoedown_buffer_grow(ib, ib->size + IUNIT) != HOEDOWN_BUF_OK) {
			fprintf(stderr, "Couldn't grow input buffer.\n");
            exit(1);
		}
		ib->size += fread(ib->data + ib->size, 1, IUNIT, in);
	}

	/* performing markdown parsing */
	ob = hoedown_buffer_new(OUNIT);
	if (!ob) {
		fprintf(stderr, "Couldn't allocate output buffer.\n");
        exit(1);
	}

	renderer = hoedown_html_renderer_new(0, 0);
	document = hoedown_document_new(renderer, extensions, 16);

	hoedown_document_render(document, ob, ib->data, ib->size);

	/* cleanup */
	hoedown_document_free(document);
	hoedown_html_renderer_free(renderer);
	hoedown_buffer_free(ib);
    fclose(in);

	return ob;
}

/* post information function */
char *
get_date(char *name)
{
    char *date, *p;
    int i = 0;

    date = malloc(14);
    p = name;
    while (*p != '.')
        date[i++] = *p++;
    date[i] = '\0';

    return date;
}

char *
get_title(hoedown_buffer *buf)
{
    uint8_t *title, *ptr;
    size_t  s, t;
    int i = 0;

    /* find start and end position of <h1> title */
    s = t = 0;
    while (s+2 < buf->size &&
            !(buf->data[s] == '<' && buf->data[s+1] == 'h' && buf->data[s+2] == '1'))
        s += 1;

    if (s+2 < buf->size) {
        s += 4;
        t = s;

        while (t+3 < buf->size &&
                !(buf->data[t] == '<' && buf->data[t+1] == '/' && buf->data[t+2] == 'h' && buf->data[t+3] == '1'))
            t++;

        if (t+3 < buf->size) {
            ptr = malloc(t - s + 1);
            title = ptr;
            while (s < t)
                ptr[i++] = buf->data[s++];
            ptr[i] = '\0';
            return (char *) title;
        }
    }

    /* no match, get default value */
    ptr = malloc(strlen(VACANT) + 1);
    title = ptr;
    while (VACANT[i] != '\0')
        *ptr++ = VACANT[i++];
    *ptr = '\0';

    return (char *) title;
}

/* build index */
void
build_index(list sen)
{
    int     i;
    char    index[256], template[256], date[11];
    char    *outline, line[MAXLINE+1];
    char    site[] = "index";
    FILE *in, *out;

    if (sen->next == NULL)
        return;

    sprintf(index, "site/index.html");
    sprintf(template, ".sim/index.html");

    in = fopen(template, "r");
    out = fopen(index, "w");

    date_sort(sen);
    while (fgets(line, MAXLINE, in) != NULL) {
        if (strstr(line, "\%title\%") != NULL) {
            outline = str_rep(line, "\%title\%", site);
            fputs(outline, out);
            free(outline);
            continue;
        }
        
        if (strstr(line, "\%content\%") != NULL) {
            fputs("<ul>\n", out);

            position p;
            p = sen->next;
            while (p != sen) {
                for (i = 0; i < 10; i++)
                    date[i] = p->data->date[i];
                date[i] = '\0';

                fprintf(out, "<li><span class=\"date\">%s</span>"
                        " <span class=\"title\"><a href=\"%s.html\">"
                        "%s</a></span></li>\n",
                        p->data->date, date, p->data->title);
                p = p->next;
            }

            fputs("</ul>\n", out);
            continue;
        }

        fputs(line, out);
    }
}

