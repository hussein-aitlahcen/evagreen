import { Injectable } from '@angular/core';
import { AuthHttp } from 'angular2-jwt';
import { Observable } from 'rxjs/Observable';

export class Agent {
    constructor(public id: Number,
        public description: String,
        public uploadInterval: Number,
        public snapshotInterval: Number,
        public widthResolution: Number,
        public heightResolution: Number,
        public initialContact: Boolean) {
    }
}

@Injectable()
export class AgentService {
    constructor(private authHttp: AuthHttp) { }

    all(): Observable<Agent[]> {
        return this.authHttp.get("/api/agent")
            .map(res => res.json());
    }
}