import { Injectable } from '@angular/core';
import { AuthHttp } from 'angular2-jwt';
import { Observable } from 'rxjs/Observable';

export class Data {
    constructor(public id: Number,
        public type: Number,
        public creationDate: Number,
        public integrationDate: Number,
        public value: String,
        public description: String) {
    }
}

@Injectable()
export class DataService {
    constructor(private authHttp: AuthHttp) { }

    all(): Observable<Data[]> {
        return this.authHttp.get("/api/data")
            .map(res => res.json());
    }
}