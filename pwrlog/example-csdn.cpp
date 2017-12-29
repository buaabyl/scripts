#include <stdio.h>
#include <string.h>
#include <time.h>

#include <windows.h>

#define BUFFER_SIZE 1024*64

//#define DEFAULT_FILE "C:\\ossec-extracted-evt.log"


FILE *fp;
int event_record=0;


/* Event logging local structure */
typedef struct _os_el
{
    int time_of_last;
    char *event_name;

    EVENTLOGRECORD *er;
    HANDLE h;

    DWORD record;
} os_el;
os_el el[3];
int el_last = 0;


/** int startEL(char *app, os_el *el)
* Starts the event logging for each el
*/
int startEL(char *app, os_el *el)
{
    /* Opening the event log */
    el->h = OpenEventLog(NULL, app);
    if (!el->h)
    {
        return(0);
    }

    el->event_name = app;
    GetOldestEventLogRecord(el->h, &el->record);

    return(1);
}



/** char *el_GetCategory(int category_id)
* Returns a string related to the category id of the log.
*/
//得到一个事件的类型，，输入是一个事件类型id输出是汉字串

char *el_GetCategory(int category_id)
{
    char *cat;
    switch (category_id)
    {
    case EVENTLOG_ERROR_TYPE:
        cat = "错误";
        break;
    case EVENTLOG_WARNING_TYPE:
        cat = "警告";
        break;
    case EVENTLOG_INFORMATION_TYPE:
        cat = "信息";
        break;
    case EVENTLOG_AUDIT_SUCCESS:
        cat = "审核成功";
        break;
    case EVENTLOG_AUDIT_FAILURE:
        cat = "审核失败";
        break;
    default:
        cat = "Unknown";
        break;
    }
    return(cat);
}


/** int el_getEventDLL(char *evt_name, char *event_sourcename, char *event)
* Returns the event.
*/
int el_getEventDLL(char *evt_name, char *event_sourcename, char *event)
{
    HKEY key;
    DWORD ret;
    char keyname[256];


    keyname[255] = '\0';

    _snprintf(keyname, 254,
              "System\\CurrentControlSet\\Services\\EventLog\\%s\\%s",
              evt_name,
              event_sourcename);

    /* Opening registry */
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, keyname, 0, KEY_ALL_ACCESS, &key)
            != ERROR_SUCCESS)
    {
        return(0);
    }


    ret = MAX_PATH -1;
    if (RegQueryValueEx(key, "EventMessageFile", NULL,
                        NULL, (LPBYTE)event, &ret) != ERROR_SUCCESS)
    {
        event[0] = '\0';
        return(0);
    }

    RegCloseKey(key);
    return(1);
}



/** char *el_Getmessage()
* Returns a descriptive message of the event.
*/
//输出是时间描述信息

char *el_GetMessage(EVENTLOGRECORD *er, char *event_name,
                    char * event_sourcename, LPTSTR *el_sstring)
{
    DWORD fm_flags = 0;
    char tmp_str[257];
    char event[MAX_PATH +1];
    char *curr_str;
    char *next_str;
    LPSTR message = NULL;

    HMODULE hevt;

    /* Initializing variables */
    event[MAX_PATH] = '\0';
    tmp_str[256] = '\0';

    /* Flags for format event */
    fm_flags |= FORMAT_MESSAGE_FROM_HMODULE;
    fm_flags |= FORMAT_MESSAGE_ALLOCATE_BUFFER;
    fm_flags |= FORMAT_MESSAGE_ARGUMENT_ARRAY;

    /* Get the file name from the registry (stored on event) */
    if (!el_getEventDLL(event_name, event_sourcename, event))
    {
        return(NULL);
    }

    curr_str = event;

    /* If our event has multiple libraries, try each one of them */
    while ((next_str = strchr(curr_str, ';')))
    {
        *next_str = '\0';
        next_str++;

        ExpandEnvironmentStrings(curr_str, tmp_str, 255);
        hevt = LoadLibraryEx(tmp_str, NULL, DONT_RESOLVE_DLL_REFERENCES);
        if (hevt)
        {
            if (!FormatMessage(fm_flags, hevt, er->EventID,
                               0,
                               (LPTSTR) &message, 0, el_sstring))
            {
                message = NULL;
            }
            FreeLibrary(hevt);

            /* If we have a message, we can return it */
            if (message)
                return(message);
        }

        curr_str = next_str;
    }

    ExpandEnvironmentStrings(curr_str, tmp_str, 255);
    hevt = LoadLibraryEx(tmp_str, NULL, DONT_RESOLVE_DLL_REFERENCES);
    if (hevt)
    {
        int hr;
        if (!(hr = FormatMessage(fm_flags, hevt, er->EventID,
                                 0,
                                 (LPTSTR) &message, 0, el_sstring)))
        {
            message = NULL;
        }
        FreeLibrary(hevt);

        /* If we have a message, we can return it */
        if (message)
            return(message);
    }

    return(NULL);
}



/** void Read_event(os_el *el)
* Reads the event log.
*/
void Read_event(os_el *el, int printit)
{
    DWORD nstr;
    DWORD user_size;
    DWORD domain_size;
    DWORD read, needed;
    int size_left;
    int str_size;

    char *mbuffer[BUFFER_SIZE];
    LPSTR sstr = NULL;

    char *tmp_str = NULL;
    char *event_category; //事件类型

    char *event_sourcename; //事件来源

    char *event_computername; //事件计算机名

    char *event_descriptive_msg; //事件描述


    char event_el_user[257]; //事件用户

    char event_el_domain[257]; //事件域

    char el_string[1025];
    char final_out_msg[1024]; //最后输出的信息

    LPSTR el_sstring[57];

    /* Er must point to the mbuffer */
    el->er = (EVENTLOGRECORD *) &mbuffer;

    /* Zeroing the last values */
    el_string[1024] = '\0';
    event_el_user[256] = '\0';
    event_el_domain[256] = '\0';
    final_out_msg[1023] = '\0';
    el_sstring[56] = NULL;

    /* Reading the event log */
    while (ReadEventLog(el->h,
                        EVENTLOG_FORWARDS_READ | EVENTLOG_SEQUENTIAL_READ,
                        0,
                        el->er, BUFFER_SIZE -1, &read, &needed))
    {
        while (read > 0)
        {

            /* We need to initialize every variable before the loop */
            //得到事件的类型

            event_category = el_GetCategory(el->er->EventType);
            //得到事件来源

            event_sourcename = (LPSTR) ((LPBYTE) el->er + sizeof(EVENTLOGRECORD));
            //得到计算机名

            event_computername = event_sourcename + strlen(event_sourcename) + 1;
            //给描述信息初始化

            event_descriptive_msg = NULL;


            /* 初始化domain/user尺寸 */
            user_size = 255;
            domain_size = 255;
            event_el_domain[0] = '\0';
            event_el_user[0] = '\0';


            /* 设置时间的一些描述 some description */
            if (el->er->NumStrings)
            {
                size_left = 1020;

                sstr = (LPSTR)((LPBYTE)el->er + el->er->StringOffset);
                el_string[0] = '\0';

                for (nstr = 0; nstr < el->er->NumStrings; nstr++)
                {
                    str_size = strlen(sstr);
                    strncat(el_string, sstr, size_left);

                    tmp_str= strchr(el_string, '\0');
                    if (tmp_str)
                    {
                        *tmp_str = ' ';
                        tmp_str++;
                        *tmp_str = '\0';
                    }
                    size_left-=str_size + 1;

                    if (nstr <= 54)
                        el_sstring[nstr] = (LPSTR)sstr;

                    sstr = strchr( (LPSTR)sstr, '\0');
                    sstr++;

                }

                /* Get a more descriptive message (if available) */
                event_descriptive_msg = el_GetMessage(el->er, el->event_name,
                                                      event_sourcename, el_sstring);
                if (event_descriptive_msg != NULL)
                {
                    /* Remove any \n or \r */
                    tmp_str = event_descriptive_msg;
                    while ((tmp_str = strchr(tmp_str, '\n')))
                    {
                        *tmp_str = ' ';
                        tmp_str++;



                    }

                    tmp_str = event_descriptive_msg;
                    while ((tmp_str = strchr(tmp_str, '\r')))
                    {
                        *tmp_str = ' ';
                        tmp_str++;
                        //strchr(tmp_str, '\n');


                    }
                }
            }
            else
            {
                strncpy(el_string, "(no message)", 1020);
            }


            /* 得到username */
            if (el->er->UserSidLength)
            {
                SID_NAME_USE account_type;
                if (!LookupAccountSid(NULL, (SID *)((LPSTR)el->er + el->er->UserSidOffset),
                                      event_el_user, &user_size, event_el_domain,
                                      &domain_size, &account_type))
                {
                    strncpy(event_el_user, "(no user)", 255);
                    strncpy(event_el_domain, "no domain", 255);
                }

            }

            else
            {
                strncpy(event_el_user, "A", 255);
                strncpy(event_el_domain, "N", 255);
            }

            if (printit)
            {
                tm *event_time = localtime((const long *)&el->er->TimeWritten);

                _snprintf(final_out_msg, 1022,
                          "事件记录序号:%d\n事件:%s\n日期:%.4hd-%.2hd-%.2hd\n时间: %.2hd:%.2hd:%.2hd\n事件类型:%s\n事件来源:%s\n事件ID:(%u)\n用户:%s/%s\n计算机:%s\n描述: \n%s\n\n\n",
                          event_record,
                          el->event_name,
                          event_time->tm_year + 1900,
                          event_time->tm_mon + 1,
                          event_time->tm_mday,
                          event_time->tm_hour,
                          event_time->tm_min,
                          event_time->tm_sec,
                          event_category,
                          event_sourcename,

                          (WORD)el->er->EventID,
                          event_el_domain,
                          event_el_user,

                          event_computername,
                          event_descriptive_msg != NULL?event_descriptive_msg:el_string);

                fprintf(fp, "%s\n", final_out_msg);
            }

            if (event_descriptive_msg != NULL)
                LocalFree(event_descriptive_msg);

            /* Changing the point to the er */
            read -= el->er->Length;
            el->er = (EVENTLOGRECORD *)((LPBYTE) el->er + el->er->Length);
            event_record++;
        }

        /* Setting er to the beginning of the buffer */
        el->er = (EVENTLOGRECORD *)&mbuffer;

    }
}


/** void win_startel()
* Starts the event logging for windows
*/
void win_startel(char *eventlog)
{
    event_record=1;
    startEL(eventlog, &el[el_last]);
    Read_event(&el[el_last],1);
    el_last++;
}
////////////////////////////////////////////////////////

////main////////////////////////////////////////////////

////////////////////////////////////////////////////////

int main()
{
    fp = fopen("C:\\Documents and Settings\\Administrator\\桌面\\Read_log.txt","w");
    if (!fp)
    {
        printf("Unable to open file\n");
        exit(1);
    }
    else
    {
        printf("open file\n");
    }


    fprintf(fp, "\n\n***********安全日志***********\n\n\n");
    win_startel("Security");
    printf("安全日志打开\n");

    fprintf(fp, "\n\n***********应用日志***********\n\n\n");
    win_startel("Application");
    printf("应用日志打开\n");


    fprintf(fp, "\n\n***********系统日志***********\n\n\n");
    win_startel("System");
    printf("系统日志打开\n");





    fclose(fp);
    if (fclose(fp))
    {
        printf("关闭了\n");
    }
    else
    {
        printf("还开着呢了，，抓紧关它吧\n");
    }
    return(0);
}
