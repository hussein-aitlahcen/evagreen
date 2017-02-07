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
var router_1 = require('@angular/router');
var user_service_1 = require('../shared/user.service');
var LoginResult;
(function (LoginResult) {
    LoginResult[LoginResult["Success"] = 0] = "Success";
    LoginResult[LoginResult["WrongCredentials"] = 1] = "WrongCredentials";
})(LoginResult || (LoginResult = {}));
var LoginComponent = (function () {
    function LoginComponent(user, router) {
        this.user = user;
        this.router = router;
        this.username = "";
        this.password = "";
        this.error = false;
        this.loggingIn = false;
    }
    LoginComponent.prototype.ngOnInit = function () {
        if (this.user.isLoggedIn()) {
            this.router.navigate(['/dashboard']);
        }
    };
    LoginComponent.prototype.login = function () {
        var _this = this;
        this.error = false;
        this.loggingIn = true;
        this.user.login(this.username, this.password)
            .subscribe(function (success) {
            _this.loginResult(LoginResult.Success);
        }, function (error) {
            _this.loginResult(LoginResult.WrongCredentials);
        });
    };
    LoginComponent.prototype.loginResult = function (reason) {
        switch (reason) {
            case LoginResult.Success:
                this.router.navigate(['/dashboard']);
                break;
            case LoginResult.WrongCredentials:
                this.error = true;
                break;
        }
        this.loggingIn = false;
    };
    LoginComponent = __decorate([
        core_1.Component({
            moduleId: module.id,
            selector: 'login',
            templateUrl: 'login.component.html',
            styleUrls: ['login.component.css']
        }), 
        __metadata('design:paramtypes', [user_service_1.UserService, router_1.Router])
    ], LoginComponent);
    return LoginComponent;
})();
exports.LoginComponent = LoginComponent;
//# sourceMappingURL=login.component.js.map