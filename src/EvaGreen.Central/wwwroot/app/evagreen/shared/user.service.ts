import { Injectable } from '@angular/core';
import { Http, Headers } from '@angular/http';
import { tokenNotExpired } from 'angular2-jwt';

export const AUTH_TOKEN_NAME = 'access_token';

@Injectable()
export class UserService {

    constructor(private http: Http) {}

    login(username: string, password: string) {
        const headers = new Headers();
        headers.append('Content-Type', 'application/x-www-form-urlencoded');
        return this.http
            .post('/api/authentication/login', "username=" + username + "&password=" + password, { headers })
            .map(res => res.json())
            .map(res => {
                localStorage.setItem(AUTH_TOKEN_NAME, res.access_token);
            });
    }

    logout() {
        localStorage.removeItem(AUTH_TOKEN_NAME);
    }

    isLoggedIn() {
        return tokenNotExpired(AUTH_TOKEN_NAME);
    }
}