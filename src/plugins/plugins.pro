isEmpty(SKIP_HYDROGEN_PLUGIN) {
    SUBDIRS += hydrogen
}
isEmpty(SKIP_JACK_PLUGIN) {
    SUBDIRS += jack
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
