#ifndef CONFIG_H
#define CONFIG_H
#include <QMap>
#include <QPixmap>
#include <QString>

enum FileState{
    not_open = 0,
    open_not_save = 1,
    open_save = 2
};
#define FILESTATE_TO_STR(fs) \
    (((fs) == not_open)?"not open":\
    (((fs) == open_not_save)?"open not save":\
    "open_save"))

#endif // CONFIG_H
