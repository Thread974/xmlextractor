//
//  main.c
//  xmlextractor
//
//  Created by fredo on 10/03/2016.
//  Copyright © 2016 dalleau.re. All rights reserved.
//

#include <stdio.h>
#include <string.h>
#include <libxml/xmlstring.h>
#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

struct node_info {
    xmlChar *name;
    float lat;
    float lon;
    float ele;
    int peak;
};

static struct node_info g_node_info;

void processNode(xmlTextReaderPtr reader) {
    xmlChar *name, *value; // This is UTF8 encoded string, use BAD_CAST to cast from ANSI char *
    /* handling of a node in the tree */
    name = xmlTextReaderName(reader);
    if (name == NULL)
        name = xmlStrdup(BAD_CAST "--");
    value = xmlTextReaderValue(reader);
    
    /*
     <node id="26863762" lat="44.1209581" lon="3.5815802" version="13" timestamp="2013-02-03T00:25:28Z" changeset="14890502" uid="16038" user="krysst">
     <tag k="ele" v="1565"/>
     <tag k="name" v="Mont Aigoual"/>
     <tag k="natural" v="peak"/>
     </node>
     */
    if (!xmlStrcmp(name, BAD_CAST "tag")) {
        xmlChar *k;
        xmlChar *v;
        k = xmlTextReaderGetAttribute(reader, BAD_CAST "k");
        v = xmlTextReaderGetAttribute(reader, BAD_CAST "v");
        /*
        printf("%d %d %s %d k:%s v:%s",
               xmlTextReaderDepth(reader),
               xmlTextReaderNodeType(reader),
               name,
               xmlTextReaderIsEmptyElement(reader),
               k ? k : BAD_CAST "NULL", v ? v : BAD_CAST "NULL");
        */
        if (value == NULL) {
            /*
            printf("\n");
             */
        } else {
            /*
            printf(" %s\n", value);
             */
            xmlFree(value);
        }
        
        if (!xmlStrcmp(k, BAD_CAST "name")) {
            if (g_node_info.name)
                xmlFree(g_node_info.name);
            g_node_info.name = xmlStrdup(v);
        }

        if (!xmlStrcmp(k, BAD_CAST "ele")) {
            g_node_info.ele = atof((char *)v);
        }

        if (!xmlStrcmp(k, BAD_CAST "natural") && !xmlStrcmp(v, BAD_CAST "peak")) {
            g_node_info.peak = 1;
        }
    } else if (!xmlStrcmp(name, BAD_CAST "node")) {
        if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_ELEMENT) {
            xmlChar *lat;
            xmlChar *lon;
            lat = xmlTextReaderGetAttribute(reader, BAD_CAST "lat");
            lon = xmlTextReaderGetAttribute(reader, BAD_CAST "lon");
            if (lat && lon) {
                g_node_info.lat = atof((char *)lat);
                g_node_info.lon = atof((char *)lon);
            }
        }
        if (xmlTextReaderNodeType(reader) == XML_READER_TYPE_END_ELEMENT) {
            if (g_node_info.peak) {
                printf("{ \"%s\", %f, %f, %f },\n", g_node_info.name, g_node_info.lat, g_node_info.lon, g_node_info.ele);
                if (g_node_info.name)
                    xmlFree(g_node_info.name);
                memset(&g_node_info, 0, sizeof(g_node_info));
            } else {
                /*
                printf("no peak found\n");
                 */
            }
        }
    } else {
        /*
        printf("%d %d %s\n",
               xmlTextReaderDepth(reader),
               xmlTextReaderNodeType(reader),
               name);
         */
    }

    xmlFree(name);
}

int streamFile(const char *filename) {
    xmlTextReaderPtr reader;
    int ret;
    
    reader = xmlNewTextReaderFilename(filename);
    if (reader != NULL) {
        ret = xmlTextReaderRead(reader);
        while (ret == 1) {
            processNode(reader);
            ret = xmlTextReaderRead(reader);
        }
        xmlFreeTextReader(reader);
        if (ret != 0) {
            printf("%s : failed to parse\n", filename);
        }
    } else {
        printf("Unable to open %s\n", filename);
    }

    return ret;
}

int main(int argc, const char * argv[]) {
    const char *arg;

    // insert code here...
    if (argc < 2) {
        arg = "geo.xml";
    } else {
        arg = argv[1];
    }

    streamFile(arg);
    return 0;
}
