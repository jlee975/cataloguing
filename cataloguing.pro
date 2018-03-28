TEMPLATE = subdirs

SUBDIRS += \
    marc \
    ui

ui.depends = marc
