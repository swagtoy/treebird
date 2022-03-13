/* BEGIN Status_test */

struct mstdnt_status status_test = {
    .account = {
        .avatar = "/treebird_logo.png",
        .display_name = "Test user",
        .acct = "user@test.com",
    },
    .content = "Hello world<br><br>Hi",
        
};
before_html = construct_status(&status_test, NULL);
stat_html = construct_status(&status_test, NULL);
after_html = construct_status(&status_test, NULL);

/* END Status_test */
