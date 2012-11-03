isEmpty(SKIP_HYDROGEN_PLUGIN) {
    SUBDIRS += hydrogen
}
isEmpty(SKIP_JACK_PLUGIN) {
    SUBDIRS += jack
}
isEmpty(SKIP_LV2_PLUGIN) {
    SUBDIRS += lv2
}
isEmpty(SKIP_PORTMEDIA_PLUGIN) {
    SUBDIRS += portmedia
}
isEmpty(SKIP_REVERSER_PLUGIN) {
    SUBDIRS += reverser
}
isEmpty(SKIP_SAMPLE_LOADER_PLUGIN) {
    SUBDIRS += sampleloader
}
isEmpty(SKIP_SFZ_PLUGIN) {
    SUBDIRS += sfz
}
isEmpty(SKIP_TRIMMER_PLUGIN) {
    SUBDIRS += trimmer
}
isEmpty(SKIP_ZONE_GENERATOR_PLUGIN) {
    SUBDIRS += zonegenerator
}

TEMPLATE = subdirs
