"use strict";
var __decorate = (this && this.__decorate) || function (decorators, target, key, desc) {
    var c = arguments.length, r = c < 3 ? target : desc === null ? desc = Object.getOwnPropertyDescriptor(target, key) : desc, d;
    if (typeof Reflect === "object" && typeof Reflect.decorate === "function") r = Reflect.decorate(decorators, target, key, desc);
    else for (var i = decorators.length - 1; i >= 0; i--) if (d = decorators[i]) r = (c < 3 ? d(r) : c > 3 ? d(target, key, r) : d(target, key)) || r;
    return c > 3 && r && Object.defineProperty(target, key, r), r;
};
var __metadata = (this && this.__metadata) || function (k, v) {
    if (typeof Reflect === "object" && typeof Reflect.metadata === "function") return Reflect.metadata(k, v);
};
var core_1 = require('@angular/core');
var platform_browser_1 = require('@angular/platform-browser');
var http_1 = require('@angular/http');
var forms_1 = require('@angular/forms');
var router_1 = require('@angular/router');
var ng_bootstrap_1 = require('@ng-bootstrap/ng-bootstrap');
var angular2_jwt_1 = require('angular2-jwt');
var routes_1 = require('./routes');
var main_component_1 = require('./evagreen/main.component');
var login_component_1 = require('./evagreen/login/login.component');
var dashboard_component_1 = require('./evagreen/dashboard/dashboard.component');
var logged_in_guard_1 = require('./evagreen/shared/logged-in.guard');
var user_service_1 = require('./evagreen/shared/user.service');
var image_service_1 = require('./evagreen/shared/image.service');
require('rxjs/Rx');
var AppModule = (function () {
    function AppModule(appRef) {
        this.appRef = appRef;
    }
    AppModule.prototype.ngDoBootstrap = function () {
        this.appRef.bootstrap(main_component_1.AppMain);
    };
    AppModule = __decorate([
        core_1.NgModule({
            imports: [
                ng_bootstrap_1.NgbModule.forRoot(),
                platform_browser_1.BrowserModule,
                forms_1.FormsModule,
                http_1.HttpModule,
                router_1.RouterModule.forRoot(routes_1.APP_ROUTES)
            ],
            declarations: [
                main_component_1.AppMain,
                login_component_1.LoginComponent,
                dashboard_component_1.DashboardComponent
            ],
            providers: [
                angular2_jwt_1.provideAuth({
                    headerName: 'Authorization',
                    headerPrefix: 'Bearer',
                    tokenName: user_service_1.AUTH_TOKEN_NAME,
                    tokenGetter: function () { return localStorage.getItem(user_service_1.AUTH_TOKEN_NAME); },
                    globalHeaders: [{ 'Content-Type': 'application/json' }],
                    noJwtError: true,
                    noTokenScheme: true
                }),
                logged_in_guard_1.LoggedInGuard,
                user_service_1.UserService,
                image_service_1.ImageService
            ],
            entryComponents: [
                main_component_1.AppMain
            ]
        }), 
        __metadata('design:paramtypes', [core_1.ApplicationRef])
    ], AppModule);
    return AppModule;
}());
exports.AppModule = AppModule;
//# sourceMappingURL=app.module.js.map