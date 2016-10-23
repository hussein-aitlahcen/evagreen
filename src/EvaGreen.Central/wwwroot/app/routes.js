"use strict";
var login_component_1 = require('./evagreen/login/login.component');
var dashboard_component_1 = require('./evagreen/dashboard/dashboard.component');
var logged_in_guard_1 = require('./evagreen/shared/logged-in.guard');
exports.APP_ROUTES = [
    { path: '', component: login_component_1.LoginComponent },
    { path: 'login', component: login_component_1.LoginComponent },
    { path: 'dashboard', component: dashboard_component_1.DashboardComponent, canActivate: [logged_in_guard_1.LoggedInGuard] }
];
//# sourceMappingURL=routes.js.map