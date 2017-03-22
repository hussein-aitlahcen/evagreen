import { Injectable } from '@angular/core';
import { AuthHttp } from 'angular2-jwt';
import { Observable } from 'rxjs/Observable';


export class Data {

    public static DATA_IMAGE = 1;
    public static DATA_TEMPERATURE = 2;

    constructor(public id: Number,
        public agentId: Number,
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

    all(agentId: Number): Observable<Data[]> {
        return this.authHttp.get("/api/data/agent/" + agentId)
            .map(res => res.json());
    }
}