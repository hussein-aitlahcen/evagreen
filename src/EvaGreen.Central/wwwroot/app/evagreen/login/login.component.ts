import { Component } from '@angular/core';
import { Router } from '@angular/router';

import { UserService } from '../shared/user.service';

enum LoginResult {
    Success = 0,
    WrongCredentials = 1,
}

@Component({
    moduleId: module.id,
    selector: 'login',
    templateUrl: 'login.component.html',
    styles: [
        'fieldset { border: 0; text-align: center; max-width: 400px; margin: auto; }',
        '.form-signin { margin-top: 50px; }',
        '.form-signin-control { margin-top: 5px; margin-bottom: 5px; }'
    ]
})
export class LoginComponent {

    username: string;
    password: string;
    error: boolean;
    loggingIn: boolean;

    constructor(private user: UserService, private router: Router) {
        this.username = "";
        this.password = "";
        this.error = false;
        this.loggingIn = false;
    }

    ngOnInit() {
        if (this.user.isLoggedIn()) {
            this.router.navigate(['/dashboard']);
        }
    }

    login() {
        this.error = false;
        this.loggingIn = true;
        this.user.login(this.username, this.password)
            .subscribe(success => {
                this.loginResult(LoginResult.Success);
            }, error => {
                this.loginResult(LoginResult.WrongCredentials);
            });
    }

    loginResult(reason: LoginResult) {
        switch (reason) {
            case LoginResult.Success:
                this.router.navigate(['/dashboard']);
                break;
            case LoginResult.WrongCredentials:
                this.error = true;
                break;
        }
        this.loggingIn = false;
    }
}