TEMPLATE = subdirs

SUBDIRS += \
    marc \
    ui \
    jlee

marc.depends = jlee
ui.depends = marc
