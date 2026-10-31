#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_JOBS        200
#define NAME_LEN        50
#define PHONE_LEN       15
#define DEVICE_LEN      30
#define PROBLEM_LEN     100
#define DATA_FILE       "jobs.dat"
#define SITE_NAME       "MR. FIX IT"
#define STYLESHEET      "/repair/style.css"

#define STAFF_PASSWORD  "fixit2026"
#define COOKIE_NAME     "rjt_staff"

#define MAX_PARAMS      30
#define KEY_LEN         32
#define VALUE_LEN       512
#define MAX_INPUT       8192

#define CARDS_PER_LANE  6

typedef enum {
    RECEIVED = 1,
    DIAGNOSING,
    WAITING_PARTS,
    IN_REPAIR,
    COMPLETED,
    COLLECTED
} Status;

typedef struct {
    int day;
    int month;
    int year;
} Date;

typedef struct {
    int    jobId;
    char   customerName[NAME_LEN];
    char   phone[PHONE_LEN];
    char   deviceType[DEVICE_LEN];
    char   brand[DEVICE_LEN];
    char   problem[PROBLEM_LEN];
    char   technician[NAME_LEN];
    float  estimatedCost;
    float  amountPaid;
    Status status;
    Date   dateIn;
} Job;

typedef struct {
    char key[KEY_LEN];
    char value[VALUE_LEN];
} Param;

typedef struct {
    const char *name;
    const char *shapes;
} Icon;

const Icon icons[] = {
    { "wrench",   "<path d='M14.7 6.3a1 1 0 0 0 0 1.4l1.6 1.6a1 1 0 0 0 1.4 0l3.77-3.77a6 6 0 0 1-7.94 "
                  "7.94l-6.91 6.91a2.12 2.12 0 0 1-3-3l6.91-6.91a6 6 0 0 1 7.94-7.94l-3.76 3.76z'/>" },
    { "plus",     "<path d='M12 5v14M5 12h14'/>" },
    { "back",     "<path d='M19 12H5M12 19l-7-7 7-7'/>" },
    { "forward",  "<path d='M5 12h14M12 5l7 7-7 7'/>" },
    { "search",   "<circle cx='11' cy='11' r='7'/><path d='M20 20l-3.5-3.5'/>" },
    { "download", "<path d='M21 15v4a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2v-4M7 10l5 5 5-5M12 15V3'/>" },
    { "trash",    "<path d='M3 6h18M8 6V4a1 1 0 0 1 1-1h6a1 1 0 0 1 1 1v2M19 6l-1 14a2 2 0 0 1-2 "
                  "2H8a2 2 0 0 1-2-2L5 6M10 11v6M14 11v6'/>" },
    { "wallet",   "<path d='M19 7V4a1 1 0 0 0-1-1H5a2 2 0 0 0 0 4h15a1 1 0 0 1 1 1v4h-3a2 2 0 0 0 "
                  "0 4h3a1 1 0 0 0 1-1v-2a1 1 0 0 0-1-1'/><path d='M3 5v14a2 2 0 0 0 2 2h15a1 1 0 "
                  "0 0 1-1v-4'/>" },
    { "refresh",  "<path d='M21 12a9 9 0 0 1-15.5 6.2L3 16M3 21v-5h5M3 12a9 9 0 0 1 15.5-6.2L21 8M21 3v5h-5'/>" },
    { "check",    "<circle cx='12' cy='12' r='10'/><path d='M8 12.5l2.5 2.5L16 9.5'/>" },
    { "tick",     "<path d='M5 12.5l4.5 4.5L19 7.5'/>" },
    { "alert",    "<circle cx='12' cy='12' r='10'/><path d='M12 8v4M12 16h.01'/>" },
    { "clipboard","<rect x='8' y='2' width='8' height='4' rx='1'/><path d='M16 4h2a2 2 0 0 1 2 "
                  "2v14a2 2 0 0 1-2 2H6a2 2 0 0 1-2-2V6a2 2 0 0 1 2-2h2M9 12h6M9 16h4'/>" },
    { "package",  "<path d='M21 8l-9-5-9 5 9 5 9-5zM3 8v8l9 5 9-5V8M12 13v8'/>" },
    { "chart",    "<path d='M3 3v18h18M8 17v-4M13 17V8M18 17v-7'/>" },
    { "inbox",    "<path d='M22 12h-6l-2 3h-4l-2-3H2'/><path d='M5.5 5.1L2 12v6a2 2 0 0 0 2 2h16a2 2 0 0 0 "
                  "2-2v-6l-3.5-6.9A2 2 0 0 0 16.7 4H7.3a2 2 0 0 0-1.8 1.1z'/>" },
    { "clock",    "<circle cx='12' cy='12' r='10'/><path d='M12 6v6l4 2'/>" },
    { "lock",     "<rect x='4' y='11' width='16' height='10' rx='2'/><path d='M8 11V7a4 4 0 0 1 8 0v4'/>" },
    { "logout",   "<path d='M9 21H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h4M16 17l5-5-5-5M21 12H9'/>" },
    { "shield",   "<path d='M12 22s8-4 8-10V5l-8-3-8 3v7c0 6 8 10 8 10z'/><path d='M9 12l2 2 4-4'/>" },
    { "laptop",   "<rect x='4' y='4' width='16' height='11' rx='2'/><path d='M2 19h20'/>" },
    { "monitor",  "<rect x='2' y='3' width='20' height='14' rx='2'/><path d='M8 21h8M12 17v4'/>" },
    { "printer",  "<path d='M6 9V3h12v6'/><rect x='3' y='9' width='18' height='8' rx='2'/>"
                  "<path d='M7 14h10v7H7z'/>" },
    { "phone",    "<rect x='6' y='2' width='12' height='20' rx='2'/><path d='M11 18h2'/>" },
    { "tablet",   "<rect x='4' y='2' width='16' height='20' rx='2'/><path d='M11 18h2'/>" }
};
const int iconCount = sizeof(icons) / sizeof(icons[0]);

Job   jobs[MAX_JOBS];
int   jobCount = 0;
int   nextJobId = 1001;

Param params[MAX_PARAMS];
int   paramCount = 0;

const char *scriptName;
int mainOpen = 0;
int staff = 0;

void readRequest(void);
void parseParams(char *data);
void urlDecode(char *s);
const char *getParam(const char *key);
int  hexValue(char c);

unsigned long hashText(const char *text);
void makeToken(char *token);
int  isStaff(void);
int  requireStaff(void);
void pageLogin(int wrongPassword);
void actionLogin(void);
void actionLogout(void);

void pagePublicHome(void);
void pageTrack(void);

void pageWorkshop(void);
void pageJobList(void);
void pageNewJob(void);
void pageViewJob(void);
void pageReport(void);
void pageSlip(void);
void exportReport(void);

void actionAddJob(void);
void actionUpdateStatus(void);
void actionPayment(void);
void actionDelete(void);

void startPage(const char *title);
void endPage(void);
void pageHeading(const char *eyebrow, const char *title, const char *subtitle);
void printNavLink(const char *action, const char *label);
void printEscaped(const char *s);
void printStatusBadge(Status s);
void printStatusOptions(Status selected);
void printBoard(void);
void printJobCard(const Job *j);
void printFloatCard(int position, const char *iconName, const char *title,
                    const char *line1, Status s);
void printTrackForm(int inHero);
void printTrackResult(const Job *j, int full);
void printDays(int days);
void printIcon(const char *name);
const char *deviceIcon(const char *deviceType);
const char *stageIcon(Status s);
const char *stageMessage(Status s);
void startMessage(int isError);
void endMessage(void);
void showError(const char *message);
void redirect(const char *query);

int   findJobById(int id);
const char *statusName(Status s);
float balanceDue(const Job *j);
const char *money(float amount);
int   parseAmount(const char *text, float *amount);
int   parseJobNumber(const char *text);
void  normalizePhone(const char *src, char *dest);
int   phoneMatches(const char *a, const char *b);
void  copyField(char *dest, const char *src, int size);
void  toLowerCase(const char *src, char *dest, int size);
void  getInitials(const char *name, char *initials);
void  getTodayDate(Date *d);
int   daysSince(const Date *d);
void  sortJobs(int option);

void loadJobs(void);
void saveJobs(void);

int main(void)
{
    const char *method, *action;

    scriptName = getenv("SCRIPT_NAME");
    if (scriptName == NULL)
        scriptName = "repair.exe";

    readRequest();
    loadJobs();
    staff = isStaff();

    method = getenv("REQUEST_METHOD");
    action = getParam("action");

    if (method != NULL && strcmp(method, "POST") == 0) {
        if (strcmp(action, "login") == 0) {
            actionLogin();
        } else if (requireStaff()) {
            if      (strcmp(action, "add") == 0)    actionAddJob();
            else if (strcmp(action, "status") == 0) actionUpdateStatus();
            else if (strcmp(action, "pay") == 0)    actionPayment();
            else if (strcmp(action, "delete") == 0) actionDelete();
            else showError("Unknown action.");
        }
        return 0;
    }

    if (strcmp(action, "") == 0)       { pagePublicHome(); return 0; }
    if (strcmp(action, "track") == 0)  { pageTrack();      return 0; }
    if (strcmp(action, "logout") == 0) { actionLogout();   return 0; }
    if (strcmp(action, "login") == 0) {
        if (staff)
            redirect("?action=workshop");
        else
            pageLogin(0);
        return 0;
    }

    if (!requireStaff())
        return 0;

    if      (strcmp(action, "workshop") == 0) pageWorkshop();
    else if (strcmp(action, "new") == 0)      pageNewJob();
    else if (strcmp(action, "view") == 0)     pageViewJob();
    else if (strcmp(action, "list") == 0)     pageJobList();
    else if (strcmp(action, "report") == 0)   pageReport();
    else if (strcmp(action, "slip") == 0)     pageSlip();
    else if (strcmp(action, "export") == 0)   exportReport();
    else pageWorkshop();

    return 0;
}

void readRequest(void)
{
    static char query[MAX_INPUT];
    static char body[MAX_INPUT];
    const char *qs = getenv("QUERY_STRING");
    const char *len = getenv("CONTENT_LENGTH");

    if (qs != NULL) {
        copyField(query, qs, MAX_INPUT);
        parseParams(query);
    }

    if (len != NULL) {
        int length = atoi(len);
        if (length > 0) {
            if (length >= MAX_INPUT)
                length = MAX_INPUT - 1;
            length = (int)fread(body, 1, length, stdin);
            body[length] = '\0';
            parseParams(body);
        }
    }
}

void parseParams(char *data)
{
    char *pair = strtok(data, "&");

    while (pair != NULL && paramCount < MAX_PARAMS) {
        char *equals = strchr(pair, '=');
        Param *p = &params[paramCount];

        if (equals != NULL) {
            *equals = '\0';
            copyField(p->key, pair, KEY_LEN);
            copyField(p->value, equals + 1, VALUE_LEN);
        } else {
            copyField(p->key, pair, KEY_LEN);
            p->value[0] = '\0';
        }

        urlDecode(p->key);
        urlDecode(p->value);
        paramCount++;

        pair = strtok(NULL, "&");
    }
}

void urlDecode(char *s)
{
    char *read = s, *write = s;

    while (*read != '\0') {
        if (*read == '+') {
            *write++ = ' ';
            read++;
        } else if (*read == '%' && hexValue(read[1]) >= 0 && hexValue(read[2]) >= 0) {
            *write++ = (char)(hexValue(read[1]) * 16 + hexValue(read[2]));
            read += 3;
        } else {
            *write++ = *read++;
        }
    }
    *write = '\0';
}

int hexValue(char c)
{
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

const char *getParam(const char *key)
{
    int i;
    for (i = 0; i < paramCount; i++)
        if (strcmp(params[i].key, key) == 0)
            return params[i].value;
    return "";
}

unsigned long hashText(const char *text)
{
    unsigned long hash = 5381;
    while (*text != '\0')
        hash = hash * 33 + (unsigned char)*text++;
    return hash;
}

void makeToken(char *token)
{
    sprintf(token, "%08lx", hashText(STAFF_PASSWORD "-repair-job-tracker"));
}

int isStaff(void)
{
    const char *cookies = getenv("HTTP_COOKIE");
    char token[20], expected[40];

    if (cookies == NULL)
        return 0;

    makeToken(token);
    sprintf(expected, "%s=%s", COOKIE_NAME, token);
    return strstr(cookies, expected) != NULL;
}

int requireStaff(void)
{
    if (staff)
        return 1;
    redirect("?action=login");
    return 0;
}

void pageLogin(int wrongPassword)
{
    startPage("Staff login");

    printf("<main class='wrap page login-page'>\n");
    mainOpen = 1;

    printf("<span class='login-icon'>");
    printIcon("lock");
    printf("</span>\n");
    printf("<span class='eyebrow'>Staff</span>\n<h1>Staff login</h1>\n");
    printf("<p class='muted'>Only workshop staff can see jobs, customers and payments.</p>\n");

    if (wrongPassword) {
        startMessage(1);
        printf("That password is not correct. Try again.");
        endMessage();
    }

    printf("<form class='panel form login-panel' method='post' action='%s'>\n", scriptName);
    printf("<input type='hidden' name='action' value='login'>\n");
    printf("<label>Password<input type='password' name='password' "
           "autocomplete='current-password' required autofocus></label>\n");
    printf("<button type='submit'>");
    printIcon("lock");
    printf("Log in</button>\n</form>\n");

    endPage();
}

void actionLogin(void)
{
    char token[20];

    if (strcmp(getParam("password"), STAFF_PASSWORD) != 0) {
        pageLogin(1);
        return;
    }

    makeToken(token);
    printf("Set-Cookie: %s=%s; Path=/; HttpOnly; SameSite=Strict\n", COOKIE_NAME, token);
    redirect("?action=workshop");
}

void actionLogout(void)
{
    printf("Set-Cookie: %s=; Path=/; Max-Age=0\n", COOKIE_NAME);
    redirect("?msg=5");
}

void pagePublicHome(void)
{
    int s;

    const char *cardIcon[]    = { "laptop", "monitor", "printer", "laptop" };
    const char *cardDevice[]  = { "HP EliteBook", "Dell Optiplex", "Epson L3150", "Lenovo ThinkPad" };
    const char *cardProblem[] = { "Screen flickering", "Will not boot", "Paper jam", "Keyboard keys dead" };
    Status cardStatus[]       = { IN_REPAIR, DIAGNOSING, COMPLETED, WAITING_PARTS };

    const char *serviceIcon[]  = { "laptop", "monitor", "printer", "phone", "shield", "package" };
    const char *serviceName[]  = { "Laptops", "Desktops & monitors", "Printers",
                                   "Phones & tablets", "Software & viruses", "Upgrades" };
    const char *serviceText[]  = {
        "Screens, keyboards, batteries, hinges, charging ports and overheating.",
        "Machines that will not start, power supplies, display faults and noisy fans.",
        "Paper jams, ink and toner systems, poor print quality and connection problems.",
        "Cracked screens, charging faults and batteries that no longer hold charge.",
        "Windows reinstalls, virus removal, slow computers and lost files.",
        "More memory, faster SSD storage and setting up new parts properly."
    };

    startPage("Computer repair workshop");

    printf("<section class='hero' id='track'>\n");
    for (s = 0; s < 4; s++)
        printFloatCard(s + 1, cardIcon[s], cardDevice[s], cardProblem[s], cardStatus[s]);

    printf("<div class='wrap hero-inner'>\n");
    if (atoi(getParam("msg")) == 5) {
        startMessage(0);
        printf("You have logged out.");
        endMessage();
    }
    printf("<span class='pill'><i></i>Laptops &middot; Desktops &middot; Printers &middot; Phones</span>\n");
    printf("<h1>Know exactly where your repair is.</h1>\n");
    printf("<p class='lead'>Enter the job number from your drop-off slip or the phone number "
           "you gave us to see how your device is coming along.</p>\n");
    printTrackForm(1);
    printf("</div>\n</section>\n");

    printf("<section class='section' id='services'>\n<div class='wrap'>\n");
    printf("<div class='section-head'>\n<span class='eyebrow'>Services</span>\n");
    printf("<h2>What we fix.</h2>\n");
    printf("<p>Bring it to the counter. We check it, tell you the cost, and only "
           "start once you agree.</p>\n</div>\n");
    printf("<div class='services'>\n");
    for (s = 0; s < 6; s++) {
        printf("<div class='service'><span class='service-icon'>");
        printIcon(serviceIcon[s]);
        printf("</span><h3>%s</h3><p>%s</p></div>\n", serviceName[s], serviceText[s]);
    }
    printf("</div>\n</div>\n</section>\n");

    printf("<section class='section' id='how'>\n<div class='wrap'>\n");
    printf("<div class='section-head'>\n<span class='eyebrow'>How it works</span>\n");
    printf("<h2>Six stages, from drop-off to pick-up.</h2>\n");
    printf("<p>Every repair moves through the same stages. When you track your "
           "repair, this is what each one means.</p>\n</div>\n");
    printf("<div class='stages'>\n");
    for (s = RECEIVED; s <= COLLECTED; s++) {
        printf("<div class='stage'><span class='stage-icon'>");
        printIcon(stageIcon((Status)s));
        printf("</span>");
        printStatusBadge((Status)s);
        printf("<p>%s</p></div>\n", stageMessage((Status)s));
    }
    printf("</div>\n</div>\n</section>\n");

    printf("<div class='wrap'>\n<div class='cta-band'>\n<div>\n");
    printf("<h2>Dropped off a device?</h2>\n");
    printf("<p>Check its progress any time, with your job number or phone.</p>\n</div>\n");
    printf("<a class='btn btn-accent btn-lg' href='%s?action=track'>", scriptName);
    printIcon("search");
    printf("Track your repair</a>\n");
    printf("</div>\n</div>\n");

    endPage();
}

void pageTrack(void)
{
    int id = parseJobNumber(getParam("id"));
    const char *phone = getParam("phone");
    char phoneDigits[64];
    int i, found = 0, fullCount = 0, idMatch, phoneMatch;

    normalizePhone(phone, phoneDigits);

    startPage("Track your repair");
    pageHeading("Track", "Track your repair",
                "Use the job number on your drop-off slip, the phone number you gave us, or both.");

    if (id == 0 && phoneDigits[0] == '\0') {
        printf("<div class='panel narrow'>\n");
        printTrackForm(0);
        printf("</div>\n");
        endPage();
        return;
    }

    for (i = 0; i < jobCount; i++) {
        idMatch    = id > 0 && jobs[i].jobId == id;
        phoneMatch = phoneDigits[0] != '\0' && phoneMatches(jobs[i].phone, phone);
        if (idMatch || phoneMatch)
            found++;
        if (idMatch && phoneMatch)
            fullCount++;
    }

    if (found > 0 && fullCount == 0) {
        startMessage(0);
        if (id > 0 && phoneDigits[0] != '\0')
            printf("That job number and phone number belong to different repairs, "
                   "so the cost and payments are hidden.");
        else
            printf("Enter both your job number and phone number to see the "
                   "cost and payments.");
        endMessage();
    }

    for (i = jobCount - 1; i >= 0; i--) {
        idMatch    = id > 0 && jobs[i].jobId == id;
        phoneMatch = phoneDigits[0] != '\0' && phoneMatches(jobs[i].phone, phone);
        if (idMatch || phoneMatch)
            printTrackResult(&jobs[i], idMatch && phoneMatch);
    }

    if (found == 0) {
        startMessage(1);
        printf("We could not find a repair with those details. "
               "Check your slip and try again.");
        endMessage();
        printf("<div class='panel narrow'>\n");
        printTrackForm(0);
        printf("</div>\n");
        endPage();
        return;
    }

    printf("<a class='back' href='%s?action=track'>", scriptName);
    printIcon("back");
    printf("Track another repair</a>\n");

    endPage();
}

void printTrackResult(const Job *j, int full)
{
    int s;
    char firstName[NAME_LEN];
    const char *message = stageMessage(j->status);

    printf("<div class='panel narrow'>\n");
    printf("<span class='eyebrow'>Job #%d</span>\n", j->jobId);

    if (full) {
        copyField(firstName, j->customerName, NAME_LEN);
        firstName[strcspn(firstName, " ")] = '\0';
        printf("<p class='status-note'>Hi ");
        printEscaped(firstName);
        printf(", %s</p>\n", message);
    } else {
        printf("<p class='status-note'>%c%s</p>\n",
               toupper((unsigned char)message[0]), message + 1);
    }

    printf("<p class='muted'>");
    printEscaped(j->deviceType);
    printf(" &middot; ");
    printEscaped(j->brand);
    printf(" &middot; booked in %02d/%02d/%04d</p>\n",
           j->dateIn.day, j->dateIn.month, j->dateIn.year);

    printf("<div class='tracker'>\n");
    for (s = RECEIVED; s <= COLLECTED; s++) {
        const char *state = s < (int)j->status ? "done" : (s == (int)j->status ? "now" : "next");
        printf("<div class='track-step %s'><span class='dot'>", state);
        printIcon(s < (int)j->status ? "tick" : stageIcon((Status)s));
        printf("</span>%s</div>\n", statusName((Status)s));
    }
    printf("</div>\n");

    if (full) {
        printf("<table class='details'>\n");
        printf("<tr><th>Problem reported</th><td>");
        printEscaped(j->problem);
        printf("</td></tr>\n");
        printf("<tr><th>Repair cost</th><td>KSh %s</td></tr>\n", money(j->estimatedCost));
        printf("<tr><th>Paid so far</th><td>KSh %s</td></tr>\n", money(j->amountPaid));
        printf("<tr class='total'><th>Balance to pay</th><td>KSh %s</td></tr>\n",
               money(balanceDue(j)));
        printf("</table>\n");
    }

    printf("</div>\n");
}

void printTrackForm(int inHero)
{
    printf("<form class='%s' method='get' action='%s'>\n",
           inHero ? "track-bar" : "form", scriptName);
    printf("<input type='hidden' name='action' value='track'>\n");

    if (inHero) {
        printf("<input name='id' placeholder='Job number, e.g. 1001' inputmode='numeric' "
               "aria-label='Job number'>\n");
        printf("<span class='divider'></span>\n");
        printf("<input name='phone' type='tel' placeholder='or phone number' "
               "aria-label='Phone number'>\n");
    } else {
        printf("<div class='row'>\n");
        printf("<label>Job number<input name='id' placeholder='e.g. 1001' inputmode='numeric' "
               "value='");
        printEscaped(getParam("id"));
        printf("'></label>\n");
        printf("<label>Phone number<input name='phone' type='tel' placeholder='e.g. 0712 345 678' "
               "value='");
        printEscaped(getParam("phone"));
        printf("'></label>\n</div>\n");
        printf("<p class='muted form-hint'>Fill in one or both. With both, you also see the "
               "cost and payments.</p>\n");
    }

    printf("<div><button type='submit' class='btn-accent'>");
    printIcon("search");
    printf("Track repair</button></div>\n</form>\n");
}

void pageWorkshop(void)
{
    int i, k, inShop = 0, working = 0, ready = 0, owingCount = 0;
    int owing[MAX_JOBS];
    float outstanding = 0, received = 0;
    char initials[3];

    for (i = 0; i < jobCount; i++) {
        if (jobs[i].status != COLLECTED)
            inShop++;
        if (jobs[i].status >= DIAGNOSING && jobs[i].status <= IN_REPAIR)
            working++;
        if (jobs[i].status == COMPLETED)
            ready++;
        if (balanceDue(&jobs[i]) > 0)
            owing[owingCount++] = i;
        outstanding += balanceDue(&jobs[i]);
        received += jobs[i].amountPaid;
    }

    for (i = 0; i < owingCount - 1; i++) {
        int biggest = i, temp;
        for (k = i + 1; k < owingCount; k++)
            if (balanceDue(&jobs[owing[k]]) > balanceDue(&jobs[owing[biggest]]))
                biggest = k;
        temp = owing[i];
        owing[i] = owing[biggest];
        owing[biggest] = temp;
    }

    startPage("Workshop");
    pageHeading("Staff", "Workshop", "Every device in the shop, by stage.");

    printf("<div class='stats'>\n");
    printf("<div class='stat'><span class='stat-icon'>");
    printIcon("laptop");
    printf("</span><strong>%d</strong><span>Devices in the shop</span></div>\n", inShop);
    printf("<div class='stat'><span class='stat-icon'>");
    printIcon("wrench");
    printf("</span><strong>%d</strong><span>Being worked on</span></div>\n", working);
    printf("<div class='stat'><span class='stat-icon'>");
    printIcon("package");
    printf("</span><strong>%d</strong><span>Ready for collection</span></div>\n", ready);
    printf("<div class='stat'><span class='stat-icon'>");
    printIcon("wallet");
    printf("</span><strong>%s</strong><span>Owed by customers (KSh)</span></div>\n",
           money(outstanding));
    printf("</div>\n");

    printf("<h3 class='block-title'>Workshop board</h3>\n");
    printBoard();

    printf("<div class='cols cols-even block'>\n");
    printf("<div class='panel'>\n<h3 class='with-icon'>");
    printIcon("wallet");
    printf("Balances due</h3>\n");
    if (owingCount == 0) {
        printf("<p class='muted'>Everyone has paid up. Nothing is owed right now.</p>\n");
    } else {
        for (i = 0; i < owingCount && i < 5; i++) {
            Job *j = &jobs[owing[i]];
            getInitials(j->customerName, initials);
            printf("<a class='owing' href='%s?action=view&amp;id=%d'>"
                   "<span class='avatar'>%s</span><div><b>", scriptName, j->jobId, initials);
            printEscaped(j->customerName);
            printf("</b><small>#%d &middot; ", j->jobId);
            printEscaped(j->deviceType);
            printf("</small></div><strong>KSh %s</strong></a>\n", money(balanceDue(j)));
        }
    }
    printf("</div>\n");

    printf("<div class='panel'>\n<h3 class='with-icon'>");
    printIcon("chart");
    printf("Money</h3>\n<table class='details'>\n");
    printf("<tr><th>Received so far</th><td class='num'>KSh %s</td></tr>\n", money(received));
    printf("<tr class='total'><th>Still owed</th><td class='num'>KSh %s</td></tr>\n",
           money(outstanding));
    printf("</table>\n<p><a class='btn btn-ghost' href='%s?action=report'>", scriptName);
    printIcon("chart");
    printf("Open the shop report</a></p>\n</div>\n");
    printf("</div>\n");

    endPage();
}

void pageJobList(void)
{
    const char *q = getParam("q");
    int filter = atoi(getParam("status"));
    int sort = atoi(getParam("sort"));
    int msg = atoi(getParam("msg"));
    char qLower[NAME_LEN], nameLower[NAME_LEN];
    int i, shown = 0;

    if (sort >= 1 && sort <= 3)
        sortJobs(sort);

    startPage("All jobs");
    pageHeading("Staff", "All jobs", "Search, filter and sort every job the shop has booked in.");

    if (msg == 4) {
        startMessage(0);
        printf("Job deleted.");
        endMessage();
    }

    printf("<form class='filters' method='get' action='%s'>\n", scriptName);
    printf("<input type='hidden' name='action' value='list'>\n");
    printf("<input type='text' name='q' placeholder='Search customer name' value='");
    printEscaped(q);
    printf("'>\n<select name='status'><option value='0'>All stages</option>\n");
    printStatusOptions((Status)filter);
    printf("</select>\n<select name='sort'>\n");
    printf("<option value='0'>Order booked in</option>\n");
    printf("<option value='1'%s>Job number</option>\n", sort == 1 ? " selected" : "");
    printf("<option value='2'%s>Customer name (A-Z)</option>\n", sort == 2 ? " selected" : "");
    printf("<option value='3'%s>Balance due (highest)</option>\n", sort == 3 ? " selected" : "");
    printf("</select>\n<button type='submit'>Apply</button>\n");
    printf("<a class='btn btn-ghost' href='%s?action=list'>Clear</a>\n</form>\n", scriptName);

    printf("<div class='panel flush'>\n<table class='list jobs-table'>\n");
    printf("<tr><th>Job</th><th>Date in</th><th>Customer</th><th>Phone</th>"
           "<th>Device</th><th>Stage</th><th class='num'>Cost</th>"
           "<th class='num'>Balance</th></tr>\n");

    toLowerCase(q, qLower, NAME_LEN);

    for (i = 0; i < jobCount; i++) {
        Job *j = &jobs[i];

        toLowerCase(j->customerName, nameLower, NAME_LEN);
        if (strstr(nameLower, qLower) == NULL)
            continue;
        if (filter != 0 && j->status != (Status)filter)
            continue;

        printf("<tr><td><a href='%s?action=view&amp;id=%d'>#%d</a></td>",
               scriptName, j->jobId, j->jobId);
        printf("<td>%02d/%02d/%04d</td><td>", j->dateIn.day, j->dateIn.month, j->dateIn.year);
        printEscaped(j->customerName);
        printf("</td><td>");
        printEscaped(j->phone);
        printf("</td><td>");
        printEscaped(j->deviceType);
        printf(" &middot; ");
        printEscaped(j->brand);
        printf("</td><td>");
        printStatusBadge(j->status);
        printf("</td><td class='num'>%s</td><td class='num'>%s</td></tr>\n",
               money(j->estimatedCost), money(balanceDue(j)));
        shown++;
    }

    if (shown == 0)
        printf("<tr><td colspan='8' class='empty'>No jobs found.</td></tr>\n");
    printf("</table>\n</div>\n");

    endPage();
}

void pageNewJob(void)
{
    startPage("Book in a device");
    pageHeading("Staff", "Book in a device",
                "Fill in the details while the customer is at the counter.");

    printf("<form class='panel form' method='post' action='%s'>\n", scriptName);
    printf("<input type='hidden' name='action' value='add'>\n");

    printf("<div class='form-section'>\n<h3>Customer</h3>\n<div class='row'>\n");
    printf("<label>Full name<input name='name' maxlength='%d' required></label>\n", NAME_LEN - 1);
    printf("<label>Phone number<input name='phone' type='tel' maxlength='%d' required></label>\n",
           PHONE_LEN - 1);
    printf("</div>\n</div>\n");

    printf("<div class='form-section'>\n<h3>Device</h3>\n<div class='row'>\n");
    printf("<label>Type<select name='device'>"
           "<option>Laptop</option><option>Desktop</option><option>Printer</option>"
           "<option>Monitor</option><option>Phone</option><option>Tablet</option>"
           "<option>Other</option></select></label>\n");
    printf("<label>Brand / model<input name='brand' maxlength='%d' required></label>\n", DEVICE_LEN - 1);
    printf("</div>\n");
    printf("<label>Problem reported<textarea name='problem' maxlength='%d' required></textarea></label>\n",
           PROBLEM_LEN - 1);
    printf("<label>Technician<input name='tech' maxlength='%d' required></label>\n", NAME_LEN - 1);
    printf("</div>\n");

    printf("<div class='form-section'>\n<h3>Payment</h3>\n<div class='row'>\n");
    printf("<label>Estimated cost (KSh)<input name='cost' type='number' min='0' step='0.01' required></label>\n");
    printf("<label>Deposit paid (KSh)<input name='deposit' type='number' min='0' step='0.01' value='0'></label>\n");
    printf("</div>\n</div>\n");

    printf("<div><button type='submit' class='btn-lg'>");
    printIcon("plus");
    printf("Book in device</button></div>\n");
    printf("</form>\n");

    endPage();
}

void pageViewJob(void)
{
    int id = atoi(getParam("id"));
    int index = findJobById(id);
    int msg = atoi(getParam("msg"));
    Job *j;
    char title[40];

    if (index == -1) {
        showError("That job was not found.");
        return;
    }

    j = &jobs[index];
    sprintf(title, "Job #%d", j->jobId);
    startPage(title);

    printf("<section class='page-head'>\n<div class='wrap'>\n<span class='eyebrow'>");
    printStatusBadge(j->status);
    printf("</span>\n<h1>");
    printEscaped(j->customerName);
    printf("</h1>\n<p>Job #%d &middot; ", j->jobId);
    printEscaped(j->deviceType);
    printf(" &middot; ");
    printEscaped(j->brand);
    printf(" &middot; in the shop ");
    printDays(daysSince(&j->dateIn));
    printf("</p>\n</div>\n</section>\n<main class='wrap page'>\n");
    mainOpen = 1;

    if (msg >= 1 && msg <= 3) {
        startMessage(0);
        switch (msg) {
            case 1:
                printf("Job booked in as #%d. <a href='%s?action=slip&amp;id=%d'>"
                       "Print the drop-off slip</a> for the customer.",
                       j->jobId, scriptName, j->jobId);
                break;
            case 2: printf("Stage updated to %s.", statusName(j->status)); break;
            case 3: {
                float change = (float)atof(getParam("change"));
                printf("Payment recorded.");
                if (change > 0)
                    printf(" Give the customer change of KSh %s.", money(change));
                break;
            }
        }
        endMessage();
    }

    printf("<div class='cols'>\n");

    printf("<div class='panel'>\n<h3 class='with-icon'>");
    printIcon(deviceIcon(j->deviceType));
    printf("Job details</h3>\n<table class='details'>\n");
    printf("<tr><th>Date in</th><td>%02d/%02d/%04d</td></tr>\n",
           j->dateIn.day, j->dateIn.month, j->dateIn.year);
    printf("<tr><th>Phone</th><td>");      printEscaped(j->phone);      printf("</td></tr>\n");
    printf("<tr><th>Problem</th><td>");    printEscaped(j->problem);    printf("</td></tr>\n");
    printf("<tr><th>Technician</th><td>"); printEscaped(j->technician); printf("</td></tr>\n");
    printf("<tr><th>Stage</th><td>");      printStatusBadge(j->status); printf("</td></tr>\n");
    printf("<tr><th>Cost</th><td>KSh %s</td></tr>\n", money(j->estimatedCost));
    printf("<tr><th>Paid</th><td>KSh %s</td></tr>\n", money(j->amountPaid));
    printf("<tr class='total'><th>Balance due</th><td>KSh %s</td></tr>\n", money(balanceDue(j)));
    printf("</table>\n");
    printf("<p><a class='btn btn-ghost' href='%s?action=slip&amp;id=%d'>", scriptName, j->jobId);
    printIcon("printer");
    printf("Print drop-off slip</a></p>\n</div>\n");

    printf("<div>\n");

    printf("<form class='panel form' method='post' action='%s'>\n", scriptName);
    printf("<h3 class='with-icon'>");
    printIcon("refresh");
    printf("Move to another stage</h3>\n");
    printf("<input type='hidden' name='action' value='status'>\n");
    printf("<input type='hidden' name='id' value='%d'>\n", j->jobId);
    printf("<label>Stage<select name='status'>\n");
    printStatusOptions(j->status);
    printf("</select></label>\n");
    printf("<label>New cost estimate (optional)<input name='cost' type='number' "
           "min='0' step='0.01' placeholder='%.2f'></label>\n", j->estimatedCost);
    printf("<div><button type='submit'>");
    printIcon("refresh");
    printf("Update stage</button></div>\n</form>\n");

    printf("<div class='panel'>\n<h3 class='with-icon'>");
    printIcon("wallet");
    printf("Record a payment</h3>\n");
    if (balanceDue(j) > 0) {
        printf("<form class='form' method='post' action='%s'>\n", scriptName);
        printf("<input type='hidden' name='action' value='pay'>\n");
        printf("<input type='hidden' name='id' value='%d'>\n", j->jobId);
        printf("<label>Amount received (KSh)<input name='amount' type='number' "
               "min='0.01' step='0.01' required></label>\n");
        printf("<div><button type='submit' class='btn-accent'>");
        printIcon("check");
        printf("Record payment</button></div>\n</form>\n");
    } else {
        printf("<p class='muted'>This job is fully paid.</p>\n");
    }
    printf("</div>\n");

    printf("<form method='post' action='%s' "
           "onsubmit=\"return confirm('Delete job #%d?');\">\n", scriptName, j->jobId);
    printf("<input type='hidden' name='action' value='delete'>\n");
    printf("<input type='hidden' name='id' value='%d'>\n", j->jobId);
    printf("<button type='submit' class='btn-danger'>");
    printIcon("trash");
    printf("Delete this job</button>\n</form>\n");

    printf("</div>\n</div>\n");
    printf("<a class='back' href='%s?action=workshop'>", scriptName);
    printIcon("back");
    printf("Back to the workshop</a>\n");

    endPage();
}

void pageReport(void)
{
    int i, waiting = 0, count[COLLECTED + 1] = {0};
    float totalValue = 0, totalPaid = 0;

    for (i = 0; i < jobCount; i++) {
        count[jobs[i].status]++;
        totalValue += jobs[i].estimatedCost;
        totalPaid  += jobs[i].amountPaid;
    }

    startPage("Shop report");
    pageHeading("Staff", "Shop report", "How many jobs are at each stage, and where the money stands.");

    printf("<div class='cols cols-even'>\n");

    printf("<div class='panel'>\n<h3 class='with-icon'>");
    printIcon("chart");
    printf("Jobs by stage</h3>\n<table class='details'>\n");
    for (i = RECEIVED; i <= COLLECTED; i++) {
        printf("<tr><th>");
        printStatusBadge((Status)i);
        printf("</th><td class='num'>%d</td></tr>\n", count[i]);
    }
    printf("<tr class='total'><th>Total</th><td class='num'>%d</td></tr>\n"
           "</table>\n</div>\n", jobCount);

    printf("<div class='panel'>\n<h3 class='with-icon'>");
    printIcon("wallet");
    printf("Money (KSh)</h3>\n<table class='details'>\n");
    printf("<tr><th>Total value of work</th><td class='num'>%s</td></tr>\n", money(totalValue));
    printf("<tr><th>Money received</th><td class='num'>%s</td></tr>\n", money(totalPaid));
    printf("<tr class='total'><th>Outstanding</th><td class='num'>%s</td></tr>\n",
           money(totalValue - totalPaid));
    printf("</table>\n</div>\n</div>\n");

    printf("<div class='panel flush'>\n<table class='list'>\n");
    printf("<tr><th>Ready but not collected</th><th>Customer</th><th>Phone</th>"
           "<th class='num'>Owes (KSh)</th></tr>\n");
    for (i = 0; i < jobCount; i++) {
        if (jobs[i].status != COMPLETED)
            continue;
        printf("<tr><td><a href='%s?action=view&amp;id=%d'>#%d</a></td><td>",
               scriptName, jobs[i].jobId, jobs[i].jobId);
        printEscaped(jobs[i].customerName);
        printf("</td><td>");
        printEscaped(jobs[i].phone);
        printf("</td><td class='num'>%s</td></tr>\n", money(balanceDue(&jobs[i])));
        waiting++;
    }
    if (waiting == 0)
        printf("<tr><td colspan='4' class='empty'>No devices are waiting to be collected.</td></tr>\n");
    printf("</table>\n</div>\n");

    printf("<a class='btn' href='%s?action=export'>", scriptName);
    printIcon("download");
    printf("Download report.txt</a>\n");

    endPage();
}

void pageSlip(void)
{
    int index = findJobById(atoi(getParam("id")));
    const char *host = getenv("HTTP_HOST");
    Job *j;

    if (index == -1) {
        showError("That job was not found.");
        return;
    }
    j = &jobs[index];
    if (host == NULL)
        host = "localhost";

    startPage("Drop-off slip");
    printf("<main class='wrap page'>\n");
    mainOpen = 1;

    printf("<div class='slip-actions no-print'>\n");
    printf("<a class='back' href='%s?action=view&amp;id=%d'>", scriptName, j->jobId);
    printIcon("back");
    printf("Back to job #%d</a>\n", j->jobId);
    printf("<button type='button' onclick='window.print()'>");
    printIcon("printer");
    printf("Print slip</button>\n</div>\n");

    printf("<div class='slip'>\n");
    printf("<div class='slip-head'><span class='brand'><span class='logo'>");
    printIcon("wrench");
    printf("</span>%s</span>"
           "<span class='muted'>Drop-off slip</span></div>\n", SITE_NAME);

    printf("<div class='slip-number'><span>Job number</span><strong>%d</strong></div>\n", j->jobId);

    printf("<table class='details'>\n");
    printf("<tr><th>Date in</th><td>%02d/%02d/%04d</td></tr>\n",
           j->dateIn.day, j->dateIn.month, j->dateIn.year);
    printf("<tr><th>Customer</th><td>");  printEscaped(j->customerName); printf("</td></tr>\n");
    printf("<tr><th>Phone</th><td>");     printEscaped(j->phone);        printf("</td></tr>\n");
    printf("<tr><th>Device</th><td>");    printEscaped(j->deviceType);
    printf(" &middot; ");                 printEscaped(j->brand);        printf("</td></tr>\n");
    printf("<tr><th>Problem</th><td>");   printEscaped(j->problem);      printf("</td></tr>\n");
    printf("<tr><th>Estimated cost</th><td>KSh %s</td></tr>\n", money(j->estimatedCost));
    printf("<tr><th>Deposit paid</th><td>KSh %s</td></tr>\n", money(j->amountPaid));
    printf("<tr class='total'><th>Balance on collection</th><td>KSh %s</td></tr>\n",
           money(balanceDue(j)));
    printf("</table>\n");

    printf("<div class='slip-track'>\n<b>Track your repair online</b>\n");
    printf("<p>Go to <b>http://%s/repair/</b> and enter job number <b>%d</b> "
           "and the phone number above.</p>\n</div>\n", host, j->jobId);
    printf("<p class='slip-foot'>Bring this slip when you collect your device.</p>\n");
    printf("</div>\n");

    endPage();
}

void exportReport(void)
{
    int i;
    Date today;

    getTodayDate(&today);

    printf("Content-Type: text/plain\n");
    printf("Cache-Control: no-store\n");
    printf("Content-Disposition: attachment; filename=\"report.txt\"\n\n");

    printf("%s - REPAIR REPORT\n", SITE_NAME);
    printf("Generated on: %02d/%02d/%04d\n\n", today.day, today.month, today.year);
    printf("%-6s %-20s %-12s %-15s %-15s %10s %10s\n",
           "ID", "Customer", "Device", "Brand", "Status", "Cost", "Balance");
    printf("----------------------------------------------------"
           "-------------------------------------\n");

    for (i = 0; i < jobCount; i++) {
        printf("%-6d %-20s %-12s %-15s %-15s %10.2f %10.2f\n",
               jobs[i].jobId, jobs[i].customerName, jobs[i].deviceType,
               jobs[i].brand, statusName(jobs[i].status),
               jobs[i].estimatedCost, balanceDue(&jobs[i]));
    }
}

void actionAddJob(void)
{
    Job *j;
    float cost, deposit = 0;
    char query[40];

    if (jobCount >= MAX_JOBS) {
        showError("Storage full. Cannot add more jobs.");
        return;
    }

    if (getParam("name")[0] == '\0' || getParam("phone")[0] == '\0' ||
        getParam("device")[0] == '\0' || getParam("brand")[0] == '\0' ||
        getParam("problem")[0] == '\0' || getParam("tech")[0] == '\0') {
        showError("Please fill in all the fields.");
        return;
    }
    if (!parseAmount(getParam("cost"), &cost)) {
        showError("Please enter a valid estimated cost.");
        return;
    }
    if (getParam("deposit")[0] != '\0' && !parseAmount(getParam("deposit"), &deposit)) {
        showError("Please enter a valid deposit.");
        return;
    }
    if (deposit > cost) {
        showError("The deposit cannot be more than the estimated cost.");
        return;
    }

    j = &jobs[jobCount];
    j->jobId = nextJobId++;
    copyField(j->customerName, getParam("name"), NAME_LEN);
    copyField(j->phone, getParam("phone"), PHONE_LEN);
    copyField(j->deviceType, getParam("device"), DEVICE_LEN);
    copyField(j->brand, getParam("brand"), DEVICE_LEN);
    copyField(j->problem, getParam("problem"), PROBLEM_LEN);
    copyField(j->technician, getParam("tech"), NAME_LEN);
    j->estimatedCost = cost;
    j->amountPaid = deposit;
    j->status = RECEIVED;
    getTodayDate(&j->dateIn);

    jobCount++;
    saveJobs();

    sprintf(query, "?action=view&id=%d&msg=1", j->jobId);
    redirect(query);
}

void actionUpdateStatus(void)
{
    int index = findJobById(atoi(getParam("id")));
    int s = atoi(getParam("status"));
    float newCost;
    Job *j;
    char query[40];

    if (index == -1) {
        showError("That job was not found.");
        return;
    }
    if (s < RECEIVED || s > COLLECTED) {
        showError("Invalid status.");
        return;
    }

    j = &jobs[index];

    if (getParam("cost")[0] != '\0') {
        if (!parseAmount(getParam("cost"), &newCost) || newCost < j->amountPaid) {
            showError("The new cost must be a valid amount, not less than what "
                      "the customer has already paid.");
            return;
        }
        j->estimatedCost = newCost;
    }

    if (s == COLLECTED && balanceDue(j) > 0) {
        showError("This device cannot be marked as collected until the "
                  "balance is fully paid.");
        return;
    }

    j->status = (Status)s;
    saveJobs();

    sprintf(query, "?action=view&id=%d&msg=2", j->jobId);
    redirect(query);
}

void actionPayment(void)
{
    int index = findJobById(atoi(getParam("id")));
    float amount, due, change = 0;
    Job *j;
    char query[60];

    if (index == -1) {
        showError("That job was not found.");
        return;
    }
    if (!parseAmount(getParam("amount"), &amount) || amount <= 0) {
        showError("Please enter a valid payment amount.");
        return;
    }

    j = &jobs[index];
    due = balanceDue(j);

    if (amount > due) {
        change = amount - due;
        amount = due;
    }

    j->amountPaid += amount;
    saveJobs();

    sprintf(query, "?action=view&id=%d&msg=3&change=%.2f", j->jobId, change);
    redirect(query);
}

void actionDelete(void)
{
    int index = findJobById(atoi(getParam("id")));
    int i;

    if (index == -1) {
        showError("That job was not found.");
        return;
    }

    for (i = index; i < jobCount - 1; i++)
        jobs[i] = jobs[i + 1];

    jobCount--;
    saveJobs();
    redirect("?action=list&msg=4");
}

void startPage(const char *title)
{
    printf("Content-Type: text/html; charset=utf-8\n");
    printf("Cache-Control: no-store\n\n");

    printf("<!DOCTYPE html>\n<html lang='en'>\n<head>\n");
    printf("<meta charset='utf-8'>\n");
    printf("<meta name='viewport' content='width=device-width, initial-scale=1'>\n");
    printf("<title>%s | %s</title>\n", title, SITE_NAME);
    printf("<link rel='preconnect' href='https://fonts.googleapis.com'>\n");
    printf("<link rel='stylesheet' href='https://fonts.googleapis.com/css2?"
           "family=Plus+Jakarta+Sans:wght@400;500;600;700;800&display=swap'>\n");
    printf("<link rel='stylesheet' href='%s'>\n", STYLESHEET);
    printf("</head>\n<body>\n");

    printf("<header class='site-header'>\n<div class='wrap nav'>\n");

    printf("<a class='brand' href='%s%s'><span class='logo'>",
           scriptName, staff ? "?action=workshop" : "");
    printIcon("wrench");
    printf("</span>%s</a>\n", SITE_NAME);

    printf("<nav class='nav-links'>\n");
    if (staff) {
        printNavLink("workshop", "Workshop");
        printNavLink("list", "All jobs");
        printNavLink("report", "Report");
    } else {
        printNavLink("track", "Track repair");
        printf("<a href='%s#services'>Services</a>\n", scriptName);
        printf("<a href='%s#how'>How it works</a>\n", scriptName);
    }
    printf("</nav>\n");

    printf("<div class='nav-right'>\n");
    if (staff) {
        printf("<form class='search' method='get' action='%s'>", scriptName);
        printIcon("search");
        printf("<input type='hidden' name='action' value='list'>"
               "<input type='search' name='q' placeholder='Search customers'></form>\n");
        printf("<a class='btn btn-accent' href='%s?action=new'>", scriptName);
        printIcon("plus");
        printf("Book in device</a>\n");
        printf("<a class='btn btn-ghost icon-btn' href='%s?action=logout' title='Log out' "
               "aria-label='Log out'>", scriptName);
        printIcon("logout");
        printf("</a>\n");
    } else {
        printf("<a class='btn btn-ghost' href='%s?action=login'>", scriptName);
        printIcon("lock");
        printf("Staff login</a>\n");
    }
    printf("</div>\n</div>\n</header>\n");
}

void pageHeading(const char *eyebrow, const char *title, const char *subtitle)
{
    printf("<section class='page-head'>\n<div class='wrap'>\n");
    printf("<span class='eyebrow'>%s</span>\n<h1>%s</h1>\n<p>%s</p>\n", eyebrow, title, subtitle);
    printf("</div>\n</section>\n<main class='wrap page'>\n");
    mainOpen = 1;
}

void endPage(void)
{
    Date today;

    getTodayDate(&today);

    if (mainOpen)
        printf("</main>\n");

    printf("<footer class='site-footer'>\n<div class='wrap foot'>\n");
    printf("<a class='brand' href='%s'><span class='logo'>", scriptName);
    printIcon("wrench");
    printf("</span>%s</a>\n", SITE_NAME);

    printf("<nav class='foot-links'>\n");
    if (staff) {
        printf("<a href='%s?action=workshop'>Workshop</a>\n", scriptName);
        printf("<a href='%s?action=list'>All jobs</a>\n", scriptName);
        printf("<a href='%s?action=report'>Report</a>\n", scriptName);
    } else {
        printf("<a href='%s?action=track'>Track repair</a>\n", scriptName);
        printf("<a href='%s#services'>Services</a>\n", scriptName);
        printf("<a href='%s?action=login'>Staff login</a>\n", scriptName);
    }
    printf("</nav>\n");

    printf("<span class='copy'>&copy; %d %s</span>\n", today.year, SITE_NAME);
    printf("</div>\n</footer>\n</body>\n</html>\n");
}

void printNavLink(const char *action, const char *label)
{
    const char *current = getParam("action");

    if (strcmp(current, "view") == 0)
        current = "list";

    printf("<a href='%s?action=%s'%s>%s</a>\n", scriptName, action,
           strcmp(current, action) == 0 ? " class='on'" : "", label);
}

void printEscaped(const char *s)
{
    for (; *s != '\0'; s++) {
        switch (*s) {
            case '<':  printf("&lt;");   break;
            case '>':  printf("&gt;");   break;
            case '&':  printf("&amp;");  break;
            case '"':  printf("&quot;"); break;
            case '\'': printf("&#39;");  break;
            default:   putchar(*s);
        }
    }
}

void printStatusBadge(Status s)
{
    printf("<span class='st s%d'>%s</span>", (int)s, statusName(s));
}

void printStatusOptions(Status selected)
{
    int i;
    for (i = RECEIVED; i <= COLLECTED; i++)
        printf("<option value='%d'%s>%s</option>\n", i,
               i == (int)selected ? " selected" : "", statusName((Status)i));
}

void printBoard(void)
{
    int i, s, count, shown;

    printf("<div class='board'>\n");
    for (s = RECEIVED; s <= COLLECTED; s++) {
        count = 0;
        for (i = 0; i < jobCount; i++)
            if (jobs[i].status == (Status)s)
                count++;

        printf("<div class='lane'>\n<div class='lane-head'>");
        printStatusBadge((Status)s);
        printf("<span class='count'>%d</span></div>\n", count);

        shown = 0;
        for (i = jobCount - 1; i >= 0 && shown < CARDS_PER_LANE; i--) {
            if (jobs[i].status == (Status)s) {
                printJobCard(&jobs[i]);
                shown++;
            }
        }

        if (count == 0) {
            printf("<p class='lane-empty'>");
            printIcon("inbox");
            printf("Nothing here</p>\n");
        } else if (count > shown) {
            printf("<a class='more' href='%s?action=list&amp;status=%d'>%d more",
                   scriptName, s, count - shown);
            printIcon("forward");
            printf("</a>\n");
        }

        printf("</div>\n");
    }
    printf("</div>\n");
}

void printJobCard(const Job *j)
{
    printf("<a class='card' href='%s?action=view&amp;id=%d'>\n", scriptName, j->jobId);
    printf("<span class='card-top'><span>#%d</span><span>", j->jobId);
    printDays(daysSince(&j->dateIn));
    printf("</span></span>\n<b>");
    printEscaped(j->customerName);
    printf("</b>\n<span class='card-device'>");
    printIcon(deviceIcon(j->deviceType));
    printf("<span>");
    printEscaped(j->deviceType);
    printf(" &middot; ");
    printEscaped(j->brand);
    printf("</span></span>\n<span class='card-foot'><span>");
    printEscaped(j->technician);
    printf("</span>");
    if (balanceDue(j) > 0)
        printf("<span class='owes'>owes %s</span>", money(balanceDue(j)));
    else
        printf("<span class='paid'>paid</span>");
    printf("</span>\n</a>\n");
}

void printFloatCard(int position, const char *iconName, const char *title,
                    const char *line1, Status s)
{
    printf("<div class='float f%d' aria-hidden='true'>\n<span class='avatar'>", position);
    printIcon(iconName);
    printf("</span>\n<b>");
    printEscaped(title);
    printf("</b>");
    printEscaped(line1);
    printf("<br>\n");
    printStatusBadge(s);
    printf("\n</div>\n");
}

void printIcon(const char *name)
{
    int i;
    for (i = 0; i < iconCount; i++) {
        if (strcmp(icons[i].name, name) == 0) {
            printf("<svg class='icon' viewBox='0 0 24 24' fill='none' stroke='currentColor' "
                   "stroke-width='2' stroke-linecap='round' stroke-linejoin='round' "
                   "aria-hidden='true'>%s</svg>", icons[i].shapes);
            return;
        }
    }
}

const char *deviceIcon(const char *deviceType)
{
    if (strcmp(deviceType, "Laptop") == 0)  return "laptop";
    if (strcmp(deviceType, "Desktop") == 0) return "monitor";
    if (strcmp(deviceType, "Monitor") == 0) return "monitor";
    if (strcmp(deviceType, "Printer") == 0) return "printer";
    if (strcmp(deviceType, "Phone") == 0)   return "phone";
    if (strcmp(deviceType, "Tablet") == 0)  return "tablet";
    return "wrench";
}

const char *stageIcon(Status s)
{
    switch (s) {
        case RECEIVED:      return "inbox";
        case DIAGNOSING:    return "search";
        case WAITING_PARTS: return "clock";
        case IN_REPAIR:     return "wrench";
        case COMPLETED:     return "package";
        case COLLECTED:     return "check";
        default:            return "wrench";
    }
}

const char *stageMessage(Status s)
{
    switch (s) {
        case RECEIVED:      return "we have received your device and it is in the queue.";
        case DIAGNOSING:    return "a technician is finding the cause of the problem.";
        case WAITING_PARTS: return "we are waiting for a replacement part to arrive.";
        case IN_REPAIR:     return "your device is being repaired right now.";
        case COMPLETED:     return "your device is ready. Please come and collect it.";
        case COLLECTED:     return "you have collected your device. Thank you!";
        default:            return "";
    }
}

void startMessage(int isError)
{
    printf("<p class='msg%s'>", isError ? " err" : "");
    printIcon(isError ? "alert" : "check");
    printf("<span>");
}

void endMessage(void)
{
    printf("</span></p>\n");
}

void printDays(int days)
{
    if (days <= 0)
        printf("today");
    else if (days == 1)
        printf("1 day");
    else
        printf("%d days", days);
}

void showError(const char *message)
{
    startPage("Something went wrong");
    pageHeading("Error", "Something went wrong", "Nothing was saved. Go back and check the form.");
    startMessage(1);
    printf("%s", message);
    endMessage();
    printf("<a class='btn' href='javascript:history.back()'>");
    printIcon("back");
    printf("Go back</a>\n");
    endPage();
}

void redirect(const char *query)
{
    printf("Status: 303 See Other\n");
    printf("Location: %s%s\n\n", scriptName, query);
}

int findJobById(int id)
{
    int i;
    for (i = 0; i < jobCount; i++)
        if (jobs[i].jobId == id)
            return i;
    return -1;
}

const char *statusName(Status s)
{
    switch (s) {
        case RECEIVED:      return "Received";
        case DIAGNOSING:    return "Diagnosing";
        case WAITING_PARTS: return "Waiting Parts";
        case IN_REPAIR:     return "In Repair";
        case COMPLETED:     return "Completed";
        case COLLECTED:     return "Collected";
        default:            return "Unknown";
    }
}

float balanceDue(const Job *j)
{
    return j->estimatedCost - j->amountPaid;
}

const char *money(float amount)
{
    static char buffers[4][32];
    static int next = 0;
    char plain[32];
    char *out = buffers[next];
    const char *p = plain;
    int intDigits, i, k = 0;

    next = (next + 1) % 4;
    sprintf(plain, "%.2f", amount);

    if (*p == '-')
        out[k++] = *p++;

    intDigits = (int)(strchr(p, '.') - p);
    for (i = 0; p[i] != '\0'; i++) {
        if (i > 0 && i < intDigits && (intDigits - i) % 3 == 0)
            out[k++] = ',';
        out[k++] = p[i];
    }
    out[k] = '\0';
    return out;
}

int parseAmount(const char *text, float *amount)
{
    char *end;
    double value = strtod(text, &end);

    if (end == text || *end != '\0' || value < 0 || value > 10000000)
        return 0;

    *amount = (float)value;
    return 1;
}

int parseJobNumber(const char *text)
{
    while (*text != '\0' && !isdigit((unsigned char)*text))
        text++;
    return atoi(text);
}

void normalizePhone(const char *src, char *dest)
{
    char digits[64];
    int k = 0;

    for (; *src != '\0' && k < (int)sizeof(digits) - 1; src++)
        if (isdigit((unsigned char)*src))
            digits[k++] = *src;
    digits[k] = '\0';

    strcpy(dest, k > 9 ? digits + (k - 9) : digits);
}

int phoneMatches(const char *a, const char *b)
{
    char first[64], second[64];

    normalizePhone(a, first);
    normalizePhone(b, second);
    return first[0] != '\0' && strcmp(first, second) == 0;
}

void copyField(char *dest, const char *src, int size)
{
    strncpy(dest, src, size - 1);
    dest[size - 1] = '\0';
}

void toLowerCase(const char *src, char *dest, int size)
{
    int i;
    for (i = 0; src[i] != '\0' && i < size - 1; i++)
        dest[i] = (char)tolower((unsigned char)src[i]);
    dest[i] = '\0';
}

void getInitials(const char *name, char *initials)
{
    int i, k = 0, newWord = 1;

    for (i = 0; name[i] != '\0' && k < 2; i++) {
        if (isalpha((unsigned char)name[i])) {
            if (newWord)
                initials[k++] = (char)toupper((unsigned char)name[i]);
            newWord = 0;
        } else if (name[i] == ' ') {
            newWord = 1;
        }
    }
    initials[k] = '\0';
}

void getTodayDate(Date *d)
{
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    d->day   = t->tm_mday;
    d->month = t->tm_mon + 1;
    d->year  = t->tm_year + 1900;
}

int daysSince(const Date *d)
{
    struct tm then = {0};

    then.tm_mday = d->day;
    then.tm_mon  = d->month - 1;
    then.tm_year = d->year - 1900;
    then.tm_hour = 12;

    return (int)(difftime(time(NULL), mktime(&then)) / (60 * 60 * 24) + 0.5);
}

void sortJobs(int option)
{
    int i, k, swap;
    Job temp;

    for (i = 0; i < jobCount - 1; i++) {
        for (k = 0; k < jobCount - 1 - i; k++) {
            swap = 0;
            switch (option) {
                case 1:
                    swap = jobs[k].jobId > jobs[k + 1].jobId;
                    break;
                case 2:
                    swap = strcmp(jobs[k].customerName, jobs[k + 1].customerName) > 0;
                    break;
                case 3:
                    swap = balanceDue(&jobs[k]) < balanceDue(&jobs[k + 1]);
                    break;
            }
            if (swap) {
                temp = jobs[k];
                jobs[k] = jobs[k + 1];
                jobs[k + 1] = temp;
            }
        }
    }
}

void loadJobs(void)
{
    FILE *fp = fopen(DATA_FILE, "rb");

    if (fp == NULL)
        return;

    if (fread(&jobCount, sizeof(int), 1, fp) != 1 ||
        fread(&nextJobId, sizeof(int), 1, fp) != 1 ||
        jobCount < 0 || jobCount > MAX_JOBS) {
        jobCount = 0;
        nextJobId = 1001;
        fclose(fp);
        return;
    }

    jobCount = (int)fread(jobs, sizeof(Job), jobCount, fp);
    fclose(fp);
}

void saveJobs(void)
{
    FILE *fp = fopen(DATA_FILE, "wb");

    if (fp == NULL)
        return;

    fwrite(&jobCount, sizeof(int), 1, fp);
    fwrite(&nextJobId, sizeof(int), 1, fp);
    fwrite(jobs, sizeof(Job), jobCount, fp);
    fclose(fp);
}
