import { NgModule, ApplicationRef } from '@angular/core';
import { BrowserModule } from '@angular/platform-browser';
import { HttpModule } from '@angular/http';
import { FormsModule } from '@angular/forms';
import { RouterModule } from '@angular/router';
import { NgbModule } from '@ng-bootstrap/ng-bootstrap';

import { provideAuth } from 'angular2-jwt';

import { APP_ROUTES } from './routes';
import { AppMain } from './evagreen/main.component';
import { LoginComponent } from './evagreen/login/login.component';
import { DashboardComponent } from './evagreen/dashboard/dashboard.component';

import { LoggedInGuard } from './evagreen/shared/logged-in.guard';

import { UserService, AUTH_TOKEN_NAME } from './evagreen/shared/user.service';
import { DataService } from './evagreen/shared/data.service';
import 'rxjs/Rx';

@NgModule({
    imports: [
        NgbModule.forRoot(),
        BrowserModule,
        FormsModule,
        HttpModule,
        RouterModule.forRoot(APP_ROUTES)
    ],
    declarations: [
        AppMain,
        LoginComponent,
        DashboardComponent
    ],
    providers: [
        provideAuth({
            headerName: 'Authorization',
            headerPrefix: 'Bearer',
            tokenName: AUTH_TOKEN_NAME,
            tokenGetter: () => localStorage.getItem(AUTH_TOKEN_NAME),
            globalHeaders: [{ 'Content-Type': 'application/json' }],
            noJwtError: true,
            noTokenScheme: true
        }),
        LoggedInGuard,
        UserService,
        DataService
    ],
    entryComponents: [
        AppMain
    ]
})
export class AppModule {
    constructor(private appRef: ApplicationRef) { }

    ngDoBootstrap() {
        this.appRef.bootstrap(AppMain);
    }
}